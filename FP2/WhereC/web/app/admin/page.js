"use client";

import { useEffect, useState } from "react";

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

export default function AdminPage() {
  const [token, setToken] = useState("");
  const [status, setStatus] = useState("Not signed in.");
  const [theme, setTheme] = useState("dark");
  const [requests, setRequests] = useState([]);
  const [bathrooms, setBathrooms] = useState([]);
  const [reviews, setReviews] = useState([]);
  const [users, setUsers] = useState([]);
  const [editingReview, setEditingReview] = useState(null);
  const [editingBathroom, setEditingBathroom] = useState(null);
  const [reviewStatus, setReviewStatus] = useState("");
  const [bathroomStatus, setBathroomStatus] = useState("");
  const [userPasswords, setUserPasswords] = useState({});
  const [isSuperAdmin, setIsSuperAdmin] = useState(false);

  useEffect(() => {
    const storedTheme = localStorage.getItem("wherec-theme") || "dark";
    setTheme(storedTheme);
    document.body.dataset.theme = storedTheme;
  }, []);

  const toggleTheme = () => {
    const next = theme === "dark" ? "light" : "dark";
    setTheme(next);
    document.body.dataset.theme = next;
    localStorage.setItem("wherec-theme", next);
  };

  const loadAll = async (tokenValue) => {
    try {
      const [requestsData, bathroomsData, reviewsData, usersData] = await Promise.all([
        authFetchJSON(tokenValue, "/api/requests"),
        authFetchJSON(tokenValue, "/api/bathrooms"),
        authFetchJSON(tokenValue, "/api/admin/ratings"),
        authFetchJSON(tokenValue, "/api/admin/users"),
      ]);
      setRequests(requestsData);
      setBathrooms(bathroomsData);
      setReviews(reviewsData);
      setUsers(usersData);
    } catch (error) {
      setStatus(error.message);
    }
  };

  useEffect(() => {
    const stored = localStorage.getItem("wherec-token") || "";
    if (!stored) return;
    setToken(stored);
    authFetchJSON(stored, "/api/me")
      .then((data) => {
        setStatus(`Signed in as ${data.email}`);
        setIsSuperAdmin(Boolean(data.is_super_admin));
        if (data.is_admin) {
          loadAll(stored);
        } else {
          setStatus("This account is not an admin.");
        }
      })
      .catch(() => {
        localStorage.removeItem("wherec-token");
        setToken("");
      });
  }, []);

  const handleLogin = async (event) => {
    event.preventDefault();
    setStatus("");
    const formData = new FormData(event.target);
    const email = formData.get("email");
    const password = formData.get("password");
    event.target.reset();
    try {
      const result = await fetchJSON("/api/login", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ email, password }),
      });
      localStorage.setItem("wherec-token", result.token);
      setToken(result.token);
      if (!result.user.is_admin) {
        setStatus("This account is not an admin.");
        return;
      }
      const me = await authFetchJSON(result.token, "/api/me");
      setIsSuperAdmin(Boolean(me.is_super_admin));
      setStatus(`Signed in as ${result.user.email}`);
      loadAll(result.token);
    } catch (error) {
      setStatus(error.message);
    }
  };

  const handleLogout = async () => {
    setStatus("");
    try {
      if (token) {
        await authFetchJSON(token, "/api/logout", { method: "POST" });
      }
    } catch (error) {
      setStatus(error.message);
    } finally {
      localStorage.removeItem("wherec-token");
      setToken("");
      setRequests([]);
      setBathrooms([]);
      setReviews([]);
      setUsers([]);
      setEditingReview(null);
      setEditingBathroom(null);
      setIsSuperAdmin(false);
      setStatus("Not signed in.");
    }
  };

  const handleApprove = async (id) => {
    try {
      await authFetchJSON(token, `/api/requests/${id}/approve`, { method: "POST" });
      await loadAll(token);
    } catch (error) {
      setStatus(error.message);
    }
  };

  const handleReject = async (id) => {
    try {
      await authFetchJSON(token, `/api/requests/${id}/reject`, { method: "POST" });
      await loadAll(token);
    } catch (error) {
      setStatus(error.message);
    }
  };

  const handleRemoveBathroom = async (id) => {
    if (!confirm("Remove this bathroom?")) return;
    try {
      await authFetchJSON(token, `/api/bathrooms/${id}`, { method: "DELETE" });
      await loadAll(token);
    } catch (error) {
      setStatus(error.message);
    }
  };

  const handleRemoveReview = async (id) => {
    if (!confirm("Remove this review?")) return;
    try {
      await authFetchJSON(token, `/api/admin/ratings/${id}`, { method: "DELETE" });
      await loadAll(token);
    } catch (error) {
      setStatus(error.message);
    }
  };

  const handleRemoveUser = async (id, email) => {
    if (!confirm(`Remove ${email}?`)) return;
    try {
      await authFetchJSON(token, `/api/admin/users/${id}`, { method: "DELETE" });
      await loadAll(token);
    } catch (error) {
      setStatus(error.message);
    }
  };

  const handleRoleUpdate = async (id, isAdmin) => {
    try {
      await authFetchJSON(token, `/api/admin/users/${id}/role`, {
        method: "PUT",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ is_admin: isAdmin }),
      });
      await loadAll(token);
    } catch (error) {
      setStatus(error.message);
    }
  };

  const handlePasswordUpdate = async (id) => {
    const password = userPasswords[id];
    if (!password) {
      setStatus("Password required.");
      return;
    }
    try {
      await authFetchJSON(token, `/api/admin/users/${id}/password`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ password }),
      });
      setUserPasswords((prev) => ({ ...prev, [id]: "" }));
      setStatus("Password updated.");
    } catch (error) {
      setStatus(error.message);
    }
  };

  const handleReviewSave = async (event) => {
    event.preventDefault();
    if (!editingReview) return;
    const formData = new FormData(event.target);
    try {
      await authFetchJSON(token, `/api/admin/ratings/${editingReview.id}`, {
        method: "PUT",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          rating: Number(formData.get("rating")),
          comment: formData.get("comment"),
        }),
      });
      setReviewStatus("Review updated.");
      setEditingReview(null);
      await loadAll(token);
    } catch (error) {
      setReviewStatus(error.message);
    }
  };

  const handleBathroomSave = async (event) => {
    event.preventDefault();
    if (!editingBathroom) return;
    const formData = new FormData(event.target);
    const name = formData.get("name");
    const description = formData.get("description");
    try {
      await authFetchJSON(token, `/api/admin/bathrooms/${editingBathroom.id}`, {
        method: "PUT",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ name, description }),
      });
      setBathroomStatus("Bathroom updated.");
      setEditingBathroom(null);
      await loadAll(token);
    } catch (error) {
      setBathroomStatus(error.message);
    }
  };

  return (
    <div className="min-h-screen bg-[var(--bg)] text-[var(--ink)] px-6 py-8">
      <header className="relative mb-6 flex flex-wrap items-center justify-between gap-4">
        <div>
          <p className="uppercase tracking-[0.25em] text-xl font-bold text-[var(--blue)]">WhereC?</p>
          <h1 className="text-xl font-semibold text-white mt-1">Admin dashboard</h1>
        </div>
        <div className="flex flex-wrap items-center gap-3">
          <a className="ghost" href="/">
            Return to Map
          </a>
          <label className="theme-toggle" aria-label="Toggle dark mode">
            <input type="checkbox" checked={theme === "dark"} onChange={toggleTheme} />
            <span className="theme-track" />
          </label>
        </div>
      </header>

      <main className="grid gap-6 lg:grid-cols-[0.7fr_1.3fr]">
        <section className="space-y-6">
          <div className="rounded-3xl border border-[var(--border)] bg-[var(--surface)] p-4 shadow-lg">
            <h2 className="text-lg font-semibold">Admin sign in</h2>
            <form className="mt-4 grid gap-3" onSubmit={handleLogin}>
              <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
                Email
                <input
                  className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                  type="email"
                  name="email"
                  required
                  placeholder="admin@example.com"
                />
              </label>
              <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
                Password
                <input
                  className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                  type="password"
                  name="password"
                  required
                  placeholder="********"
                />
              </label>
              <div className="flex flex-wrap items-center justify-between gap-3">
                <button className="primary" type="submit">
                  Sign in
                </button>
                <div className="flex items-center gap-3 text-xs text-[var(--muted)]">
                  <span>{status}</span>
                  <button className="ghost" type="button" onClick={handleLogout}>
                    Sign out
                  </button>
                </div>
              </div>
            </form>
          </div>

          {editingReview ? (
            <div className="rounded-3xl border border-[var(--border)] bg-[var(--surface)] p-4 shadow-lg">
              <h2 className="text-lg font-semibold">Edit review</h2>
              <form className="mt-4 grid gap-3" onSubmit={handleReviewSave}>
                <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
                  Rating
                  <select
                    name="rating"
                    defaultValue={editingReview.rating}
                    className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                  >
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
                    defaultValue={editingReview.comment || ""}
                    maxLength={240}
                    className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                  />
                </label>
                <div className="flex flex-wrap gap-3">
                  <button className="primary" type="submit">
                    Save changes
                  </button>
                  <button
                    className="ghost"
                    type="button"
                    onClick={() => setEditingReview(null)}
                  >
                    Cancel
                  </button>
                </div>
                <p className="text-xs text-[var(--blue)]">{reviewStatus}</p>
              </form>
            </div>
          ) : null}

          {editingBathroom ? (
            <div className="rounded-3xl border border-[var(--border)] bg-[var(--surface)] p-4 shadow-lg">
              <h2 className="text-lg font-semibold">Edit bathroom</h2>
              <form className="mt-4 grid gap-3" onSubmit={handleBathroomSave}>
                <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
                  Name
                  <input
                    className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                    type="text"
                    name="name"
                    maxLength={80}
                    defaultValue={editingBathroom.name}
                  />
                </label>
                <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
                  Description
                  <textarea
                    name="description"
                    defaultValue={editingBathroom.description || ""}
                    maxLength={240}
                    className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                  />
                </label>
                <div className="flex flex-wrap gap-3">
                  <button className="primary" type="submit">
                    Save changes
                  </button>
                  <button
                    className="ghost"
                    type="button"
                    onClick={() => setEditingBathroom(null)}
                  >
                    Cancel
                  </button>
                </div>
                <p className="text-xs text-[var(--blue)]">{bathroomStatus}</p>
              </form>
            </div>
          ) : null}
        </section>

        <section className="space-y-6">
          <div className="rounded-3xl border border-[var(--border)] bg-[var(--surface)] p-4 shadow-lg">
            <h2 className="text-lg font-semibold">Pending requests</h2>
            <p className="text-sm text-[var(--muted)]">Approve new bathrooms submitted by users.</p>
            <div className="mt-4 space-y-3 max-h-[240px] overflow-y-auto pr-1">
              {requests.map((request) => (
                <div key={request.id} className="rounded-2xl border border-[var(--border)] bg-[var(--bg)] p-3">
                  <p className="font-semibold">{request.name}</p>
                  <p className="text-xs text-[var(--muted)]">
                    {request.requester_email || "Unknown requester"} ·{" "}
                    {request.lat.toFixed(4)}, {request.lng.toFixed(4)}
                  </p>
                  <div className="mt-2 flex flex-wrap gap-2">
                    <button className="primary" onClick={() => handleApprove(request.id)}>
                      Approve
                    </button>
                    <button className="ghost" onClick={() => handleReject(request.id)}>
                      Reject
                    </button>
                  </div>
                </div>
              ))}
            </div>
          </div>

          <div className="rounded-3xl border border-[var(--border)] bg-[var(--surface)] p-4 shadow-lg">
            <h2 className="text-lg font-semibold">Bathrooms</h2>
            <p className="text-sm text-[var(--muted)]">Edit descriptions or remove bathrooms.</p>
            <div className="mt-4 space-y-3 max-h-[240px] overflow-y-auto pr-1">
              {bathrooms.map((bathroom) => (
                <div key={bathroom.id} className="rounded-2xl border border-[var(--border)] bg-[var(--bg)] p-3">
                  <p className="font-semibold">{bathroom.name}</p>
                  <p className="text-xs text-[var(--muted)]">
                    {(bathroom.avg_rating || 0).toFixed(1)} ★ · {bathroom.rating_count} ratings
                  </p>
                  <div className="mt-2 flex flex-wrap gap-2">
                    <button className="ghost" onClick={() => setEditingBathroom(bathroom)}>
                      Edit bathroom
                    </button>
                    <button className="ghost" onClick={() => handleRemoveBathroom(bathroom.id)}>
                      Remove bathroom
                    </button>
                  </div>
                </div>
              ))}
            </div>
          </div>

          <div className="rounded-3xl border border-[var(--border)] bg-[var(--surface)] p-4 shadow-lg">
            <h2 className="text-lg font-semibold">Reviews</h2>
            <p className="text-sm text-[var(--muted)]">Edit or remove ratings.</p>
            <div className="mt-4 space-y-3 max-h-[240px] overflow-y-auto pr-1">
              {reviews.map((review) => (
                <div key={review.id} className="rounded-2xl border border-[var(--border)] bg-[var(--bg)] p-3">
                  <p className="font-semibold">{review.bathroom_name}</p>
                  <p className="text-xs text-[var(--muted)]">
                    {review.rating} ★ · {review.comment || "No comment"}
                  </p>
                  <div className="mt-2 flex flex-wrap gap-2">
                    <button className="ghost" onClick={() => setEditingReview(review)}>
                      Edit review
                    </button>
                    <button className="ghost" onClick={() => handleRemoveReview(review.id)}>
                      Remove review
                    </button>
                  </div>
                </div>
              ))}
            </div>
          </div>

          <div className="rounded-3xl border border-[var(--border)] bg-[var(--surface)] p-4 shadow-lg">
            <h2 className="text-lg font-semibold">Users</h2>
            <p className="text-sm text-[var(--muted)]">Reset passwords or remove accounts.</p>
            <div className="mt-4 space-y-3 max-h-[240px] overflow-y-auto pr-1">
              {users.map((user) => (
                <div key={user.id} className="rounded-2xl border border-[var(--border)] bg-[var(--bg)] p-3">
                  <p className="font-semibold">{user.email}</p>
                  <p className="text-xs text-[var(--muted)]">
                    {user.is_admin ? "Admin account" : "Standard account"}
                  </p>
                  {!user.is_admin ? (
                    <div className="mt-2 grid gap-2">
                      <input
                        className="w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                        type="password"
                        placeholder="New password"
                        value={userPasswords[user.id] || ""}
                        onChange={(event) =>
                          setUserPasswords((prev) => ({ ...prev, [user.id]: event.target.value }))
                        }
                      />
                      <div className="flex flex-wrap gap-2">
                        <button className="ghost" onClick={() => handlePasswordUpdate(user.id)}>
                          Change password
                        </button>
                        {isSuperAdmin ? (
                          <button className="ghost" onClick={() => handleRoleUpdate(user.id, true)}>
                            Promote to admin
                          </button>
                        ) : null}
                        <button className="ghost" onClick={() => handleRemoveUser(user.id, user.email)}>
                          Remove account
                        </button>
                      </div>
                    </div>
                  ) : (
                    <div className="mt-2 flex flex-wrap gap-2">
                      {isSuperAdmin ? (
                        <>
                          <button className="ghost" onClick={() => handleRoleUpdate(user.id, false)}>
                            Demote to user
                          </button>
                          <button className="ghost" onClick={() => handleRemoveUser(user.id, user.email)}>
                            Remove account
                          </button>
                        </>
                      ) : (
                        <span className="text-xs text-[var(--muted)]">Main admin only</span>
                      )}
                    </div>
                  )}
                </div>
              ))}
            </div>
          </div>
        </section>
      </main>
    </div>
  );
}
