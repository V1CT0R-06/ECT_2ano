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

export default function AuthPage() {
  const [token, setToken] = useState("");
  const [status, setStatus] = useState("Not signed in.");
  const [theme, setTheme] = useState("dark");

  useEffect(() => {
    const storedTheme = localStorage.getItem("wherec-theme") || "dark";
    setTheme(storedTheme);
    document.body.dataset.theme = storedTheme;
  }, []);

  useEffect(() => {
    const stored = localStorage.getItem("wherec-token") || "";
    if (!stored) return;
    setToken(stored);
    authFetchJSON(stored, "/api/me")
      .then((data) => setStatus(`Signed in as ${data.email}`))
      .catch(() => {
        localStorage.removeItem("wherec-token");
        setToken("");
      });
  }, []);

  const toggleTheme = () => {
    const next = theme === "dark" ? "light" : "dark";
    setTheme(next);
    document.body.dataset.theme = next;
    localStorage.setItem("wherec-theme", next);
  };

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
      alert("Signed in successfully.");
      window.location.href = "/";
    } catch (error) {
      setStatus(error.message);
    }
  };

  const handleRegister = async (event) => {
    event.preventDefault();
    setStatus("");
    const formData = new FormData(event.target);
    const email = formData.get("email");
    const password = formData.get("password");
    event.target.reset();
    try {
      await fetchJSON("/api/register", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ email, password }),
      });
      const result = await fetchJSON("/api/login", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ email, password }),
      });
      localStorage.setItem("wherec-token", result.token);
      setToken(result.token);
      alert("Account created and signed in.");
      window.location.href = "/";
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
      setStatus("Not signed in.");
    }
  };

  return (
    <div className="min-h-screen bg-[var(--bg)] text-[var(--ink)] px-6 py-8">
      <header className="relative mb-6 flex flex-wrap items-center justify-between gap-4">
        <div>
          <p className="uppercase tracking-[0.25em] text-xl font-bold text-[var(--blue)]">WhereC?</p>
          <h1 className="text-xl font-semibold text-white mt-1">Account access</h1>
        </div>
        <div className="flex flex-wrap items-center gap-3">
          <a className="ghost" href="/">
            Return to Map
          </a>
          <a className="ghost" href="/admin">
            Admin Dashboard
          </a>
          <label className="theme-toggle" aria-label="Toggle dark mode">
            <input type="checkbox" checked={theme === "dark"} onChange={toggleTheme} />
            <span className="theme-track" />
          </label>
        </div>
      </header>

      <main className="grid gap-6 lg:grid-cols-2">
        <section className="rounded-3xl border border-[var(--border)] bg-[var(--surface)] p-4 shadow-lg">
          <h2 className="text-lg font-semibold">Sign in</h2>
          <form className="mt-4 grid gap-3" onSubmit={handleLogin}>
            <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
              Email
              <input
                className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                type="email"
                name="email"
                required
                placeholder="you@example.com"
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
        </section>

        <section className="rounded-3xl border border-[var(--border)] bg-[var(--surface)] p-4 shadow-lg">
          <h2 className="text-lg font-semibold">Create account</h2>
          <form className="mt-4 grid gap-3" onSubmit={handleRegister}>
            <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
              Email
              <input
                className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                type="email"
                name="email"
                required
                placeholder="new@example.com"
              />
            </label>
            <label className="text-xs uppercase tracking-[0.2em] text-[var(--muted)]">
              Password
              <input
                className="mt-2 w-full rounded-xl border border-[var(--border)] bg-[var(--surface)] px-3 py-2 text-sm"
                type="password"
                name="password"
                required
                placeholder="Create a password"
              />
            </label>
            <button className="ghost w-fit" type="submit">
              Create account
            </button>
          </form>
        </section>
      </main>
    </div>
  );
}
