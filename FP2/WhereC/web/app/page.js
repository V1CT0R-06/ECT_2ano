"use client";

import { useEffect, useMemo, useRef, useState } from "react";

const API_BASE = process.env.NEXT_PUBLIC_API_BASE || "";

const fetchJSON = async (url, options) => {
  const response = await fetch(`${API_BASE}${url}`, options);
  const contentType = response.headers.get("content-type") || "";
  let data = {};
  if (contentType.includes("application/json")) {
    data = await response.json();
  } else {
    const text = await response.text();
    data = text ? { error: text } : {};
  }
  if (!response.ok) {
    throw new Error(data.error || `Request failed (${response.status})`);
  }
  return data;
};

const authFetchJSON = async (token, url, options = {}) => {
  if (!token) {
    throw new Error("Sign in required.");
  }
  const headers = Object.assign({}, options.headers, {
    Authorization: `Bearer ${token}`,
  });
  return fetchJSON(url, { ...options, headers });
};

const useTheme = () => {
  const [theme, setTheme] = useState("dark");

  useEffect(() => {
    const stored = localStorage.getItem("wherec-theme") || "dark";
    setTheme(stored);
    document.body.dataset.theme = stored;
  }, []);

  const toggleTheme = () => {
    const next = theme === "dark" ? "light" : "dark";
    setTheme(next);
    document.body.dataset.theme = next;
    localStorage.setItem("wherec-theme", next);
  };

  return { theme, toggleTheme };
};

export default function HomePage() {
  const mapRef = useRef(null);
  const mapInstanceRef = useRef(null);
  const markersRef = useRef(new Map());
  const leafletRef = useRef(null);
  const lightLayerRef = useRef(null);
  const darkLayerRef = useRef(null);

  const [bathrooms, setBathrooms] = useState([]);
  const [selectedId, setSelectedId] = useState(null);
  const [ratings, setRatings] = useState([]);
  const [ratingsStatus, setRatingsStatus] = useState("");
  const [requestMessage, setRequestMessage] = useState("");
  const [ratingMessage, setRatingMessage] = useState("");
  const [myRequests, setMyRequests] = useState([]);
  const [requestLatLng, setRequestLatLng] = useState(null);
  const [editingRequestId, setEditingRequestId] = useState(null);
  const [user, setUser] = useState(null);
  const [token, setToken] = useState("");
  const [mapReady, setMapReady] = useState(false);
  const { theme, toggleTheme } = useTheme();

  const stats = useMemo(() => {
    const totalRatings = bathrooms.reduce((sum, b) => sum + (b.rating_count || 0), 0);
    const weighted = bathrooms.reduce(
      (sum, b) => sum + (b.avg_rating || 0) * (b.rating_count || 0),
      0
    );
    return {
      count: bathrooms.length,
      avg: totalRatings ? (weighted / totalRatings).toFixed(1) : "0.0",
    };
  }, [bathrooms]);

  useEffect(() => {
    const stored = localStorage.getItem("wherec-token") || "";
    if (!stored) {
      return;
    }
    setToken(stored);
    authFetchJSON(stored, "/api/me")
      .then((data) => setUser(data))
      .catch(() => {
        localStorage.removeItem("wherec-token");
        setToken("");
      });
  }, []);

  useEffect(() => {
    fetchJSON("/api/bathrooms")
      .then((data) => {
        setBathrooms(data);
        if (data.length) {
          setSelectedId((current) => current || data[0].id);
        }
      })
      .catch(() => {});
  }, []);

  useEffect(() => {
    if (!token) {
      setMyRequests([]);
      return;
    }
    authFetchJSON(token, "/api/requests/mine")
      .then((data) => setMyRequests(data))
      .catch(() => {});
  }, [token]);

  useEffect(() => {
    let active = true;
    const loadRatings = async () => {
      if (!selectedId) {
        return;
      }
      setRatingsStatus("Loading ratings...");
      try {
        const data = token
          ? await authFetchJSON(token, `/api/bathrooms/${selectedId}/ratings`)
          : await fetchJSON(`/api/bathrooms/${selectedId}/ratings`);
        if (!active) return;
        setRatings(data);
        setRatingsStatus(data.length ? "" : "No ratings yet. Be the first!");
      } catch (error) {
        if (!active) return;
        setRatingsStatus("Unable to load ratings.");
      }
    };
    loadRatings();
    return () => {
      active = false;
    };
  }, [selectedId, token]);

  useEffect(() => {
    let map;
    let L;
    let canceled = false;
    import("leaflet").then((leaflet) => {
      if (canceled) return;
      L = leaflet;
      leafletRef.current = L;
      if (!mapRef.current || mapInstanceRef.current) {
        return;
      }

      map = L.map(mapRef.current, {
        zoomControl: false,
        maxBounds: L.latLngBounds([36.8, -9.6], [42.3, -6.0]),
        maxBoundsViscosity: 0.8,
      }).setView([39.6, -8.1], 7);

      lightLayerRef.current = L.tileLayer(
        "https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}{r}.png",
        {
          subdomains: "abcd",
          maxZoom: 19,
          attribution: "&copy; OpenStreetMap &copy; CARTO",
        }
      );
      darkLayerRef.current = L.tileLayer(
        "https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}{r}.png",
        {
          subdomains: "abcd",
          maxZoom: 19,
          attribution: "&copy; OpenStreetMap &copy; CARTO",
        }
      );

      const storedTheme = localStorage.getItem("wherec-theme") || "light";
      if (storedTheme === "dark") {
        darkLayerRef.current.addTo(map);
      } else {
        lightLayerRef.current.addTo(map);
      }

      L.control.zoom({ position: "bottomright" }).addTo(map);
      map.on("click", (event) => {
        const { lat, lng } = event.latlng;
        const latInput = document.querySelector('[name="lat"]');
        const lngInput = document.querySelector('[name="lng"]');
        if (latInput && lngInput) {
          latInput.value = lat.toFixed(4);
          lngInput.value = lng.toFixed(4);
        }
        setRequestLatLng({ lat, lng });
      });

      mapInstanceRef.current = map;
      setMapReady(true);
    });

    return () => {
      canceled = true;
      if (mapInstanceRef.current) {
        mapInstanceRef.current.remove();
        mapInstanceRef.current = null;
      }
    };
  }, []);

  useEffect(() => {
    const map = mapInstanceRef.current;
    const L = leafletRef.current;
    if (!map || !L || !mapReady) return;

    markersRef.current.forEach((marker) => map.removeLayer(marker));
    markersRef.current.clear();

    const markerIcon = L.divIcon({
      className: "bathroom-marker leaflet-div-icon",
      html: '<span class="marker-emoji">🚻</span>',
      iconSize: [32, 32],
      iconAnchor: [16, 16],
    });

    const requestIcon = L.divIcon({
      className: "request-marker leaflet-div-icon",
      html: '<span class="marker-emoji">📍</span>',
      iconSize: [30, 30],
      iconAnchor: [15, 15],
    });

    bathrooms.forEach((bathroom) => {
      const marker = L.marker([bathroom.lat, bathroom.lng], { icon: markerIcon }).addTo(map);
      marker.on("click", () => setSelectedId(bathroom.id));
      markersRef.current.set(bathroom.id, marker);
    });

    if (requestLatLng) {
      const marker = L.marker([requestLatLng.lat, requestLatLng.lng], { icon: requestIcon }).addTo(map);
      markersRef.current.set("request-preview", marker);
    }

    myRequests
      .filter((request) => request.status === "pending")
      .forEach((request) => {
        const marker = L.marker([request.lat, request.lng], { icon: requestIcon }).addTo(map);
        markersRef.current.set(`request-${request.id}`, marker);
      });
  }, [bathrooms, mapReady, requestLatLng, myRequests]);

  useEffect(() => {
    const map = mapInstanceRef.current;
    if (!map || !selectedId) return;
    const selected = bathrooms.find((item) => item.id === selectedId);
    if (selected) {
      map.setView([selected.lat, selected.lng], 15, { animate: true });
    }
  }, [selectedId, bathrooms]);

  useEffect(() => {
    const map = mapInstanceRef.current;
    if (!map || !lightLayerRef.current || !darkLayerRef.current) return;
    if (theme === "dark") {
      if (!map.hasLayer(darkLayerRef.current)) {
        map.addLayer(darkLayerRef.current);
      }
      map.removeLayer(lightLayerRef.current);
    } else {
      if (!map.hasLayer(lightLayerRef.current)) {
        map.addLayer(lightLayerRef.current);
      }
      map.removeLayer(darkLayerRef.current);
    }
  }, [theme]);

  const selectedBathroom = bathrooms.find((item) => item.id === selectedId);

  const handleRequest = async (event) => {
    event.preventDefault();
    setRequestMessage("");
    if (!token) {
      setRequestMessage("Sign in to submit requests.");
      return;
    }
    const formData = new FormData(event.target);
    const payload = {
      name: formData.get("name"),
      description: formData.get("description"),
      lat: Number(formData.get("lat")),
      lng: Number(formData.get("lng")),
    };
    try {
      await authFetchJSON(token, "/api/requests", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(payload),
      });
      setRequestMessage("Request sent for approval.");
      event.target.reset();
      setRequestLatLng(null);
      await authFetchJSON(token, "/api/requests/mine").then((data) => setMyRequests(data));
    } catch (error) {
      setRequestMessage(error.message);
    }
  };

  const handleRemoveRequest = async (id) => {
    if (!token) return;
    try {
      await authFetchJSON(token, `/api/requests/${id}`, { method: "DELETE" });
      const data = await authFetchJSON(token, "/api/requests/mine");
      setMyRequests(data);
    } catch (error) {
      setRequestMessage(error.message);
    }
  };

  const handleUpdateRequest = async (id, event) => {
    event.preventDefault();
    if (!token) return;
    const formData = new FormData(event.target);
    const payload = {
      name: formData.get("name"),
      description: formData.get("description"),
      lat: Number(formData.get("lat")),
      lng: Number(formData.get("lng")),
    };
    try {
      await authFetchJSON(token, `/api/requests/${id}`, {
        method: "PUT",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(payload),
      });
      const data = await authFetchJSON(token, "/api/requests/mine");
      setMyRequests(data);
      setEditingRequestId(null);
    } catch (error) {
      setRequestMessage(error.message);
    }
  };

  const handleRating = async (event) => {
    event.preventDefault();
    setRatingMessage("");
    if (!token || !selectedId) {
      setRatingMessage("Sign in to leave a rating.");
      return;
    }
    const formData = new FormData(event.target);
    try {
      await authFetchJSON(token, `/api/bathrooms/${selectedId}/ratings`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          rating: Number(formData.get("rating")),
          comment: formData.get("comment"),
        }),
      });
      setRatingMessage("Thanks for the rating!");
      event.target.reset();
      const data = await authFetchJSON(token, `/api/bathrooms/${selectedId}/ratings`);
      setRatings(data);
      await fetchJSON("/api/bathrooms").then((dataList) => setBathrooms(dataList));
    } catch (error) {
      setRatingMessage(error.message);
    }
  };

  const handleRatingUpdate = async (ratingId, event) => {
    event.preventDefault();
    if (!token) return;
    const formData = new FormData(event.target);
    try {
      await authFetchJSON(token, `/api/ratings/${ratingId}`, {
        method: "PUT",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          rating: Number(formData.get("rating")),
          comment: formData.get("comment"),
        }),
      });
      const data = await authFetchJSON(token, `/api/bathrooms/${selectedId}/ratings`);
      setRatings(data);
    } catch (error) {
      alert(error.message);
    }
  };

  const handleUseLocation = () => {
    if (!navigator.geolocation) {
      setRequestMessage("Geolocation not supported in this browser.");
      return;
    }
    navigator.geolocation.getCurrentPosition(
      (position) => {
        const { latitude, longitude } = position.coords;
        const latInput = document.querySelector('[name="lat"]');
        const lngInput = document.querySelector('[name="lng"]');
        if (latInput && lngInput) {
          latInput.value = latitude.toFixed(4);
          lngInput.value = longitude.toFixed(4);
        }
        setRequestLatLng({ lat: latitude, lng: longitude });
        if (mapInstanceRef.current) {
          mapInstanceRef.current.setView([latitude, longitude], 15, { animate: true });
        }
      },
      () => setRequestMessage("Unable to access location.")
    );
  };

  return (
    <div className="min-h-screen bg-[var(--bg)] text-[var(--ink)] px-6 py-8">
      <header className="relative mb-6 flex flex-wrap items-center justify-between gap-4">
        <div>
          <p className="uppercase tracking-[0.25em] text-xl font-bold text-[var(--blue)]">WhereC?</p>
          <h1 className="text-xl font-semibold text-white mt-1">Bathrooms mapped across Portugal.</h1>
          <p className="text-xs text-white/80 mt-1">
            Discover clean spots, request additions, and keep the map fresh.
          </p>
        </div>
        <div className="flex flex-wrap items-center gap-3">
          <label className="theme-toggle" aria-label="Toggle dark mode">
            <input type="checkbox" checked={theme === "dark"} onChange={toggleTheme} />
            <span className="theme-track" />
          </label>
          <div className="rounded-2xl border border-[var(--border)] bg-[var(--surface)] px-4 py-3 shadow-sm">
            <p className="text-[10px] uppercase tracking-[0.2em] text-[var(--muted)]">Live spots</p>
            <p className="text-xl font-semibold">{stats.count}</p>
          </div>
          <div className="rounded-2xl border border-[var(--border)] bg-[var(--surface)] px-4 py-3 shadow-sm">
            <p className="text-[10px] uppercase tracking-[0.2em] text-[var(--muted)]">Avg rating</p>
            <p className="text-xl font-semibold">{stats.avg}</p>
          </div>
          {user?.is_admin ? (
            <a className="ghost" href="/admin">
              Admin Dashboard
            </a>
          ) : null}
        </div>
      </header>

      <main className="grid gap-6 lg:grid-cols-[1.3fr_0.9fr]">
        <section className="rounded-3xl border border-[var(--border)] bg-[var(--surface)] p-4 shadow-lg">
          <div ref={mapRef} className="map-canvas" />
          <p className="mt-3 text-xs text-[var(--muted)]">
            Tap the map to drop a new bathroom pin.
          </p>

          <div className="mt-5 rounded-2xl border border-[var(--border)] bg-[var(--bg)] p-4">
            <h2 className="text-lg font-semibold">Request a bathroom</h2>
            <form className="mt-4 grid gap-3" onSubmit={handleRequest}>
              <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
                Name
                <input
                  className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                  type="text"
                  name="name"
                  maxLength={80}
                  required
                  placeholder="Central Library"
                />
              </label>
              <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
                Description
                <textarea
                  className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                  name="description"
                  maxLength={240}
                  placeholder="Clean, accessible, gender-neutral."
                />
              </label>
              <div className="grid gap-3 md:grid-cols-2">
                <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
                  Latitude
                  <input
                    className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                    type="number"
                    name="lat"
                    step="0.0001"
                    min="-90"
                    max="90"
                    required
                  />
                </label>
                <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
                  Longitude
                  <input
                    className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                    type="number"
                    name="lng"
                    step="0.0001"
                    min="-180"
                    max="180"
                    required
                  />
                </label>
              </div>
              <div className="flex flex-wrap gap-3">
                <button type="button" className="ghost" onClick={handleUseLocation}>
                  Use my location
                </button>
                <button type="submit" className="primary">
                  Send request
                </button>
              </div>
              <p className="text-xs text-[var(--blue)]">{requestMessage}</p>
            </form>
            <p className="mt-3 text-xs text-[var(--muted)]">
              {user ? `Signed in as ${user.email}` : "Sign in on the account page to request bathrooms."}
            </p>
            <a className="ghost mt-2 inline-flex" href="/auth">
              Sign in / Create account
            </a>
          </div>
        </section>

        <section className="space-y-6">
          <div className="rounded-3xl border border-[var(--border)] bg-[var(--surface)] p-4 shadow-lg">
            <h2 className="text-lg font-semibold">Bathrooms</h2>
            <p className="text-sm text-[var(--muted)]">Browse verified locations and ratings.</p>
            <div className="mt-4 space-y-3 max-h-[260px] overflow-y-auto pr-1">
              {bathrooms.map((bathroom) => (
                <button
                  key={bathroom.id}
                  type="button"
                  onClick={() => setSelectedId(bathroom.id)}
                  className={`w-full rounded-2xl border px-4 py-3 text-left transition ${
                    bathroom.id === selectedId
                      ? "border-[var(--blue)] bg-[rgba(31,75,153,0.08)]"
                      : "border-[var(--border)] bg-[var(--bg)] hover:border-[var(--orange)]"
                  }`}
                >
                  <p className="font-semibold">{bathroom.name}</p>
                  <p className="text-xs text-[var(--muted)]">
                    {(bathroom.avg_rating || 0).toFixed(1)} ★ · {bathroom.rating_count} ratings
                  </p>
                </button>
              ))}
            </div>
          </div>

          <div className="rounded-3xl border border-[var(--border)] bg-[var(--surface)] p-4 shadow-lg">
            <h2 className="text-lg font-semibold">My bathrooms</h2>
            <p className="text-sm text-[var(--muted)]">
              {user ? "Track your pending or rejected requests." : "Sign in to see your requests."}
            </p>
            <div className="mt-4 space-y-3 max-h-[220px] overflow-y-auto pr-1">
              {myRequests.filter((request) => request.status !== "approved").length ? (
                myRequests
                  .filter((request) => request.status !== "approved")
                  .map((request) => (
                    <div key={request.id} className="rounded-2xl border border-[var(--border)] bg-[var(--bg)] p-3">
                      {editingRequestId === request.id ? (
                        <form className="grid gap-2" onSubmit={(event) => handleUpdateRequest(request.id, event)}>
                          <input
                            className="w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                            name="name"
                            defaultValue={request.name}
                            maxLength={80}
                            required
                          />
                          <textarea
                            className="w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                            name="description"
                            defaultValue={request.description || ""}
                            maxLength={240}
                          />
                          <div className="grid gap-2 md:grid-cols-2">
                            <input
                              className="w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                              name="lat"
                              type="number"
                              step="0.0001"
                              defaultValue={request.lat}
                              required
                            />
                            <input
                              className="w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                              name="lng"
                              type="number"
                              step="0.0001"
                              defaultValue={request.lng}
                              required
                            />
                          </div>
                          <div className="flex flex-wrap gap-2">
                            <button className="primary" type="submit">
                              Save
                            </button>
                            <button
                              className="ghost"
                              type="button"
                              onClick={() => setEditingRequestId(null)}
                            >
                              Cancel
                            </button>
                          </div>
                        </form>
                      ) : (
                        <>
                          <p className="font-semibold">{request.name}</p>
                          <p className="text-xs text-[var(--muted)]">
                            {request.status} · {request.lat.toFixed(4)}, {request.lng.toFixed(4)}
                          </p>
                          <div className="mt-2 flex flex-wrap gap-2">
                            {request.status === "pending" ? (
                              <button className="ghost" type="button" onClick={() => setEditingRequestId(request.id)}>
                                Edit
                              </button>
                            ) : null}
                            <button className="ghost" type="button" onClick={() => handleRemoveRequest(request.id)}>
                              Remove
                            </button>
                          </div>
                        </>
                      )}
                    </div>
                  ))
              ) : (
                <p className="text-sm text-[var(--muted)]">
                  {user ? "No pending requests yet." : "Sign in to submit a request."}
                </p>
              )}
            </div>
          </div>

          <div className="rounded-3xl border border-[var(--border)] bg-[var(--surface)] p-4 shadow-lg">
            <h2 className="text-lg font-semibold">Bathroom details</h2>
            {selectedBathroom ? (
              <div className="mt-4 space-y-3">
                <div>
                  <h3 className="text-xl font-semibold">{selectedBathroom.name}</h3>
                  <p className="text-sm text-[var(--muted)]">
                    {selectedBathroom.description || "No description provided."}
                  </p>
                  <span className="inline-flex items-center gap-2 rounded-full bg-[var(--orange)] px-3 py-1 text-xs font-semibold text-[#2b1606]">
                    {(selectedBathroom.avg_rating || 0).toFixed(1)} ★ ({selectedBathroom.rating_count})
                  </span>
                </div>

                <form className="grid gap-3" onSubmit={handleRating}>
                  <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
                    Rating
                    <select
                      name="rating"
                      required
                      className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                    >
                      <option value="">Select</option>
                      <option value="5">5 - Excellent</option>
                      <option value="4">4 - Good</option>
                      <option value="3">3 - Okay</option>
                      <option value="2">2 - Needs work</option>
                      <option value="1">1 - Avoid</option>
                    </select>
                  </label>
                  <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
                    Comment
                    <textarea
                      name="comment"
                      maxLength={240}
                      className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                      placeholder="Share a quick note"
                    />
                  </label>
                  <button type="submit" className="primary w-fit">
                    Submit rating
                  </button>
                  <p className="text-xs text-[var(--blue)]">{ratingMessage}</p>
                </form>

                <div className="space-y-2">
                  {ratingsStatus ? (
                    <p className="text-sm text-[var(--muted)]">{ratingsStatus}</p>
                  ) : (
                    ratings.map((rating) => (
                      <div key={rating.id} className="rounded-xl border border-[var(--border)] bg-[var(--bg)] p-3 text-sm">
                        <p>
                          {rating.rating} ★ {rating.comment ? `— ${rating.comment}` : ""}
                        </p>
                        {rating.owned ? (
                          <details className="mt-2">
                            <summary className="cursor-pointer text-xs text-[var(--blue)]">
                              Edit review
                            </summary>
                            <form
                              className="mt-2 grid gap-2"
                              onSubmit={(event) => handleRatingUpdate(rating.id, event)}
                            >
                              <select
                                name="rating"
                                defaultValue={rating.rating}
                                className="w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                              >
                                <option value="5">5 - Excellent</option>
                                <option value="4">4 - Good</option>
                                <option value="3">3 - Okay</option>
                                <option value="2">2 - Needs work</option>
                                <option value="1">1 - Avoid</option>
                              </select>
                              <textarea
                                name="comment"
                                defaultValue={rating.comment || ""}
                                maxLength={240}
                                className="w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                              />
                              <button type="submit" className="primary w-fit">
                                Save changes
                              </button>
                            </form>
                          </details>
                        ) : null}
                      </div>
                    ))
                  )}
                </div>
              </div>
            ) : (
              <p className="mt-4 text-sm text-[var(--muted)]">Select a bathroom to see details.</p>
            )}
          </div>
        </section>
      </main>
    </div>
  );
}
