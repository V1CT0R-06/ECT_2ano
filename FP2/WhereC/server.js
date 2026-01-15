const path = require('path');
const fs = require('fs');
const express = require('express');
const crypto = require('crypto');
const helmet = require('helmet');
const rateLimit = require('express-rate-limit');
const sqlite3 = require('sqlite3').verbose();

const app = express();
const PORT = process.env.PORT || 3000;
const ADMIN_EMAIL = process.env.ADMIN_EMAIL;
const ADMIN_PASSWORD = process.env.ADMIN_PASSWORD;

const dataDir = path.join(__dirname, 'data');
const dbPath = path.join(dataDir, 'app.db');
if (!fs.existsSync(dataDir)) {
  fs.mkdirSync(dataDir, { recursive: true });
}

const db = new sqlite3.Database(dbPath);

const hashPassword = (password, salt = crypto.randomBytes(16).toString('hex')) => {
  const hash = crypto.scryptSync(password, salt, 64).toString('hex');
  return `${salt}:${hash}`;
};

const verifyPassword = (password, stored) => {
  const [salt, hash] = stored.split(':');
  if (!salt || !hash) {
    return false;
  }
  const derived = crypto.scryptSync(password, salt, 64).toString('hex');
  return crypto.timingSafeEqual(Buffer.from(hash, 'hex'), Buffer.from(derived, 'hex'));
};

const createToken = () => crypto.randomBytes(32).toString('hex');

db.serialize(() => {
  db.run(`
    CREATE TABLE IF NOT EXISTS bathrooms (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      name TEXT NOT NULL,
      description TEXT,
      lat REAL NOT NULL,
      lng REAL NOT NULL,
      created_at TEXT NOT NULL
    )
  `);

  db.run(`
    CREATE TABLE IF NOT EXISTS ratings (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      bathroom_id INTEGER NOT NULL,
      user_id INTEGER,
      rating INTEGER NOT NULL,
      comment TEXT,
      created_at TEXT NOT NULL,
      FOREIGN KEY (bathroom_id) REFERENCES bathrooms(id),
      FOREIGN KEY (user_id) REFERENCES users(id)
    )
  `);

  db.run(`
    CREATE TABLE IF NOT EXISTS users (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      email TEXT NOT NULL UNIQUE,
      password_hash TEXT NOT NULL,
      is_admin INTEGER NOT NULL DEFAULT 0,
      created_at TEXT NOT NULL
    )
  `);

  db.run(`
    CREATE TABLE IF NOT EXISTS bathroom_requests (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      user_id INTEGER NOT NULL,
      name TEXT NOT NULL,
      description TEXT,
      lat REAL NOT NULL,
      lng REAL NOT NULL,
      status TEXT NOT NULL,
      created_at TEXT NOT NULL,
      FOREIGN KEY (user_id) REFERENCES users(id)
    )
  `);

  db.run(`
    CREATE TABLE IF NOT EXISTS sessions (
      token TEXT PRIMARY KEY,
      user_id INTEGER NOT NULL,
      created_at TEXT NOT NULL,
      FOREIGN KEY (user_id) REFERENCES users(id)
    )
  `);

  db.all('PRAGMA table_info(ratings)', (err, rows) => {
    if (err) {
      console.error('Failed to inspect ratings table:', err);
      return;
    }
    const hasUserId = rows.some((row) => row.name === 'user_id');
    if (!hasUserId) {
      db.run('ALTER TABLE ratings ADD COLUMN user_id INTEGER');
    }
  });

  db.all('PRAGMA table_info(bathroom_requests)', (err, rows) => {
    if (err) {
      console.error('Failed to inspect bathroom_requests table:', err);
      return;
    }
    const hasUserId = rows.some((row) => row.name === 'user_id');
    if (!hasUserId) {
      db.run('ALTER TABLE bathroom_requests ADD COLUMN user_id INTEGER');
    }
  });

  if (ADMIN_EMAIL && ADMIN_PASSWORD) {
    const normalizedEmail = ADMIN_EMAIL.trim().toLowerCase();
    db.get('SELECT id FROM users WHERE email = ?', [normalizedEmail], (err, row) => {
      if (err) {
        console.error('Admin seed check failed:', err);
        return;
      }
      if (!row) {
        const passwordHash = hashPassword(ADMIN_PASSWORD);
        db.run(
          'INSERT INTO users (email, password_hash, is_admin, created_at) VALUES (?, ?, 1, ?)',
          [normalizedEmail, passwordHash, new Date().toISOString()]
        );
      } else {
        db.run('UPDATE users SET is_admin = 1 WHERE email = ?', [normalizedEmail]);
      }
    });
  }

  db.get('SELECT COUNT(*) AS count FROM bathrooms', (err, row) => {
    if (err) {
      console.error('Seed check failed:', err);
      return;
    }
    if (row.count === 0) {
      const seed = [
        {
          name: 'Aliados Metro Restrooms',
          description: 'Clean, central, and easy access near Avenida dos Aliados.',
          lat: 41.1499,
          lng: -8.6108
        },
        {
          name: 'Ribeira Public Facilities',
          description: 'Busy during peak hours, stocked and well-maintained.',
          lat: 41.1412,
          lng: -8.611
        },
        {
          name: 'Aveiro Canal Restrooms',
          description: 'Close to the canals, quick stop for visitors.',
          lat: 40.6406,
          lng: -8.6538
        },
        {
          name: 'Cais do Sodre Facilities',
          description: 'Busy but reliable restroom access near the waterfront.',
          lat: 38.7076,
          lng: -9.1447
        }
      ];

      const stmt = db.prepare(`
        INSERT INTO bathrooms (name, description, lat, lng, created_at)
        VALUES (?, ?, ?, ?, ?)
      `);
      seed.forEach((bathroom) => {
        stmt.run(
          bathroom.name,
          bathroom.description,
          bathroom.lat,
          bathroom.lng,
          new Date().toISOString()
        );
      });
      stmt.finalize();

      const ratingStmt = db.prepare(`
        INSERT INTO ratings (bathroom_id, rating, comment, created_at)
        VALUES (?, ?, ?, ?)
      `);
      ratingStmt.run(1, 5, 'Sparkling clean and easy to find.', new Date().toISOString());
      ratingStmt.run(1, 4, 'Great lighting, a little busy.', new Date().toISOString());
      ratingStmt.run(2, 3, 'Decent, could use more supplies.', new Date().toISOString());
      ratingStmt.run(3, 4, 'Nice and modern.', new Date().toISOString());
      ratingStmt.finalize();
    }
  });
});

app.use(
  helmet({
    contentSecurityPolicy: {
      useDefaults: true,
      directives: {
        "script-src": ["'self'", "https://unpkg.com"],
        "style-src": ["'self'", "https://unpkg.com", "https://fonts.googleapis.com"],
        "font-src": ["'self'", "https://fonts.gstatic.com"],
        "img-src": [
          "'self'",
          "data:",
          "https://a.basemaps.cartocdn.com",
          "https://b.basemaps.cartocdn.com",
          "https://c.basemaps.cartocdn.com",
          "https://d.basemaps.cartocdn.com"
        ],
        "connect-src": ["'self'"]
      }
    },
    crossOriginResourcePolicy: { policy: 'cross-origin' }
  })
);

app.use(
  rateLimit({
    windowMs: 60 * 1000,
    max: 120
  })
);

app.use(express.json({ limit: '100kb' }));
app.use((req, res, next) => {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET,POST,PUT,DELETE,OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');
  if (req.method === 'OPTIONS') {
    return res.sendStatus(200);
  }
  next();
});

const isNonEmptyString = (value, maxLength) =>
  typeof value === 'string' && value.trim().length > 0 && value.trim().length <= maxLength;

const isFiniteNumber = (value) => typeof value === 'number' && Number.isFinite(value);
const isWithinPortugal = (lat, lng) =>
  lat >= 36.8 && lat <= 42.3 && lng >= -9.6 && lng <= -6.0;

const getAuthUser = (req, callback) => {
  const authHeader = req.header('authorization') || '';
  const token = authHeader.startsWith('Bearer ') ? authHeader.slice(7) : null;
  if (!token) {
    return callback(null, null);
  }
  db.get(
    `SELECT users.id, users.email, users.is_admin
     FROM sessions
     JOIN users ON users.id = sessions.user_id
     WHERE sessions.token = ?`,
    [token],
    (err, row) => {
      if (err) {
        return callback(err);
      }
      if (!row) {
        return callback(null, null);
      }
      callback(null, { id: row.id, email: row.email, is_admin: row.is_admin === 1 });
    }
  );
};

const requireAuth = (req, res, next) => {
  const authHeader = req.header('authorization') || '';
  const token = authHeader.startsWith('Bearer ') ? authHeader.slice(7) : null;
  if (!token) {
    return res.status(401).json({ error: 'Authentication required.' });
  }
  db.get(
    `SELECT users.id, users.email, users.is_admin, sessions.token
     FROM sessions
     JOIN users ON users.id = sessions.user_id
     WHERE sessions.token = ?`,
    [token],
    (err, row) => {
      if (err) {
        console.error('Auth lookup failed:', err);
        return res.status(500).json({ error: 'Authentication failed.' });
      }
      if (!row) {
        return res.status(401).json({ error: 'Invalid session.' });
      }
      req.user = { id: row.id, email: row.email, is_admin: row.is_admin === 1 };
      req.sessionToken = token;
      next();
    }
  );
};

const requireAdmin = (req, res, next) => {
  requireAuth(req, res, () => {
    if (!req.user || !req.user.is_admin) {
      return res.status(403).json({ error: 'Admin authorization required.' });
    }
    next();
  });
};

app.post('/api/register', (req, res) => {
  const { email, password } = req.body || {};
  if (!isNonEmptyString(email, 120)) {
    return res.status(400).json({ error: 'Email is required.' });
  }
  if (!isNonEmptyString(password, 120)) {
    return res.status(400).json({ error: 'Password is required.' });
  }
  const normalizedEmail = email.trim().toLowerCase();
  const passwordHash = hashPassword(password);

  db.run(
    'INSERT INTO users (email, password_hash, is_admin, created_at) VALUES (?, ?, 0, ?)',
    [normalizedEmail, passwordHash, new Date().toISOString()],
    function (err) {
      if (err) {
        if (err.message && err.message.includes('UNIQUE')) {
          return res.status(409).json({ error: 'Email already registered.' });
        }
        console.error('Failed to register user:', err);
        return res.status(500).json({ error: 'Failed to register user.' });
      }
      res.status(201).json({ id: this.lastID });
    }
  );
});

app.post('/api/login', (req, res) => {
  const { email, password } = req.body || {};
  if (!isNonEmptyString(email, 120) || !isNonEmptyString(password, 120)) {
    return res.status(400).json({ error: 'Email and password are required.' });
  }
  const normalizedEmail = email.trim().toLowerCase();
  db.get('SELECT id, email, password_hash, is_admin FROM users WHERE email = ?', [normalizedEmail], (err, row) => {
    if (err) {
      console.error('Login failed:', err);
      return res.status(500).json({ error: 'Login failed.' });
    }
    if (!row || !verifyPassword(password, row.password_hash)) {
      return res.status(401).json({ error: 'Invalid credentials.' });
    }
    const token = createToken();
    db.run(
      'INSERT INTO sessions (token, user_id, created_at) VALUES (?, ?, ?)',
      [token, row.id, new Date().toISOString()],
      (insertErr) => {
        if (insertErr) {
          console.error('Session creation failed:', insertErr);
          return res.status(500).json({ error: 'Login failed.' });
        }
        res.json({ token, user: { email: row.email, is_admin: row.is_admin === 1 } });
      }
    );
  });
});

app.post('/api/logout', requireAuth, (req, res) => {
  db.run('DELETE FROM sessions WHERE token = ?', [req.sessionToken], (err) => {
    if (err) {
      console.error('Logout failed:', err);
      return res.status(500).json({ error: 'Logout failed.' });
    }
    res.json({ status: 'logged_out' });
  });
});

app.get('/api/me', requireAuth, (req, res) => {
  res.json({
    email: req.user.email,
    is_admin: req.user.is_admin,
    is_super_admin: ADMIN_EMAIL ? req.user.email === ADMIN_EMAIL : false
  });
});

app.get('/api/admin/users', requireAdmin, (req, res) => {
  db.all(
    `SELECT id, email, is_admin, created_at
     FROM users
     ORDER BY created_at DESC`,
    (err, rows) => {
      if (err) {
        console.error('Failed to fetch users:', err);
        return res.status(500).json({ error: 'Failed to load users.' });
      }
      res.json(rows);
    }
  );
});

app.get('/api/admin/ratings', requireAdmin, (req, res) => {
  db.all(
    `SELECT ratings.id, ratings.rating, ratings.comment, ratings.created_at,
            bathrooms.name AS bathroom_name
     FROM ratings
     JOIN bathrooms ON bathrooms.id = ratings.bathroom_id
     ORDER BY ratings.created_at DESC`,
    (err, rows) => {
      if (err) {
        console.error('Failed to fetch ratings:', err);
        return res.status(500).json({ error: 'Failed to load ratings.' });
      }
      res.json(rows);
    }
  );
});

app.put('/api/admin/ratings/:id', requireAdmin, (req, res) => {
  const ratingId = Number(req.params.id);
  const { rating, comment } = req.body || {};
  if (!Number.isInteger(ratingId)) {
    return res.status(400).json({ error: 'Invalid rating id.' });
  }
  if (!Number.isInteger(rating) || rating < 1 || rating > 5) {
    return res.status(400).json({ error: 'Rating must be an integer between 1 and 5.' });
  }
  if (comment !== undefined && typeof comment !== 'string') {
    return res.status(400).json({ error: 'Comment must be a string.' });
  }
  if (comment && comment.length > 240) {
    return res.status(400).json({ error: 'Comment must be 240 chars or less.' });
  }

  db.run(
    'UPDATE ratings SET rating = ?, comment = ? WHERE id = ?',
    [rating, comment ? comment.trim() : null, ratingId],
    function (err) {
      if (err) {
        console.error('Failed to update rating:', err);
        return res.status(500).json({ error: 'Failed to update rating.' });
      }
      if (this.changes === 0) {
        return res.status(404).json({ error: 'Rating not found.' });
      }
      res.json({ status: 'updated' });
    }
  );
});

app.delete('/api/admin/ratings/:id', requireAdmin, (req, res) => {
  const ratingId = Number(req.params.id);
  if (!Number.isInteger(ratingId)) {
    return res.status(400).json({ error: 'Invalid rating id.' });
  }
  db.run('DELETE FROM ratings WHERE id = ?', [ratingId], function (err) {
    if (err) {
      console.error('Failed to delete rating:', err);
      return res.status(500).json({ error: 'Failed to delete rating.' });
    }
    if (this.changes === 0) {
      return res.status(404).json({ error: 'Rating not found.' });
    }
    res.json({ status: 'deleted' });
  });
});

app.post('/api/admin/users/:id/password', requireAdmin, (req, res) => {
  const userId = Number(req.params.id);
  const { password } = req.body || {};
  if (!Number.isInteger(userId)) {
    return res.status(400).json({ error: 'Invalid user id.' });
  }
  if (!isNonEmptyString(password, 120)) {
    return res.status(400).json({ error: 'Password is required.' });
  }
  const passwordHash = hashPassword(password);
  db.run('UPDATE users SET password_hash = ? WHERE id = ?', [passwordHash, userId], function (err) {
    if (err) {
      console.error('Failed to update password:', err);
      return res.status(500).json({ error: 'Failed to update password.' });
    }
    if (this.changes === 0) {
      return res.status(404).json({ error: 'User not found.' });
    }
    res.json({ status: 'updated' });
  });
});

app.delete('/api/admin/users/:id', requireAdmin, (req, res) => {
  const userId = Number(req.params.id);
  if (!Number.isInteger(userId)) {
    return res.status(400).json({ error: 'Invalid user id.' });
  }
  if (req.user && req.user.id === userId) {
    return res.status(400).json({ error: 'Cannot delete your own account.' });
  }
  db.get('SELECT is_admin FROM users WHERE id = ?', [userId], (err, row) => {
    if (err) {
      console.error('Failed to check user:', err);
      return res.status(500).json({ error: 'Failed to delete user.' });
    }
    if (!row) {
      return res.status(404).json({ error: 'User not found.' });
    }
    if (row.is_admin === 1 && (!ADMIN_EMAIL || req.user.email !== ADMIN_EMAIL)) {
      return res.status(400).json({ error: 'Cannot delete another admin.' });
    }
    db.serialize(() => {
      db.run('DELETE FROM sessions WHERE user_id = ?', [userId]);
      db.run('DELETE FROM bathroom_requests WHERE user_id = ?', [userId]);
      db.run('DELETE FROM users WHERE id = ?', [userId], function (deleteErr) {
        if (deleteErr) {
          console.error('Failed to delete user:', deleteErr);
          return res.status(500).json({ error: 'Failed to delete user.' });
        }
        res.json({ status: 'deleted' });
      });
    });
  });
});

app.put('/api/admin/users/:id/role', requireAdmin, (req, res) => {
  const userId = Number(req.params.id);
  const { is_admin } = req.body || {};
  if (!Number.isInteger(userId)) {
    return res.status(400).json({ error: 'Invalid user id.' });
  }
  if (typeof is_admin !== 'boolean') {
    return res.status(400).json({ error: 'is_admin must be boolean.' });
  }
  if (!ADMIN_EMAIL || req.user.email !== ADMIN_EMAIL) {
    return res.status(403).json({ error: 'Only the main admin can change roles.' });
  }
  if (req.user && req.user.id === userId && !is_admin) {
    return res.status(400).json({ error: 'Cannot demote your own account.' });
  }

  db.run('UPDATE users SET is_admin = ? WHERE id = ?', [is_admin ? 1 : 0, userId], function (err) {
    if (err) {
      console.error('Failed to update role:', err);
      return res.status(500).json({ error: 'Failed to update role.' });
    }
    if (this.changes === 0) {
      return res.status(404).json({ error: 'User not found.' });
    }
    res.json({ status: 'updated' });
  });
});

app.get('/api/bathrooms', (req, res) => {
  const query = `
    SELECT b.id, b.name, b.description, b.lat, b.lng, b.created_at,
           ROUND(AVG(r.rating), 1) AS avg_rating,
           COUNT(r.id) AS rating_count
    FROM bathrooms b
    LEFT JOIN ratings r ON b.id = r.bathroom_id
    GROUP BY b.id
    ORDER BY b.created_at DESC
  `;

  db.all(query, (err, rows) => {
    if (err) {
      console.error('Failed to fetch bathrooms:', err);
      return res.status(500).json({ error: 'Failed to load bathrooms.' });
    }
    res.json(rows);
  });
});

app.get('/api/bathrooms/:id/ratings', (req, res) => {
  const bathroomId = Number(req.params.id);
  if (!Number.isInteger(bathroomId)) {
    return res.status(400).json({ error: 'Invalid bathroom id.' });
  }

  const query = `
    SELECT id, rating, comment, created_at, user_id
    FROM ratings
    WHERE bathroom_id = ?
    ORDER BY created_at DESC
  `;

  getAuthUser(req, (authErr, user) => {
    if (authErr) {
      console.error('Failed to load auth user for ratings:', authErr);
      return res.status(500).json({ error: 'Failed to load ratings.' });
    }
    db.all(query, [bathroomId], (err, rows) => {
      if (err) {
        console.error('Failed to fetch ratings:', err);
        return res.status(500).json({ error: 'Failed to load ratings.' });
      }
      const response = rows.map((row) => ({
        id: row.id,
        rating: row.rating,
        comment: row.comment,
        created_at: row.created_at,
        owned: user ? row.user_id === user.id : false
      }));
      res.json(response);
    });
  });
});

app.post('/api/requests', requireAuth, (req, res) => {
  const { name, description, lat, lng } = req.body || {};

  if (!isNonEmptyString(name, 80)) {
    return res.status(400).json({ error: 'Name is required (max 80 chars).' });
  }
  if (description !== undefined && typeof description !== 'string') {
    return res.status(400).json({ error: 'Description must be a string.' });
  }
  if (description && description.length > 240) {
    return res.status(400).json({ error: 'Description must be 240 chars or less.' });
  }
  if (!isFiniteNumber(lat) || lat < -90 || lat > 90) {
    return res.status(400).json({ error: 'Latitude must be between -90 and 90.' });
  }
  if (!isFiniteNumber(lng) || lng < -180 || lng > 180) {
    return res.status(400).json({ error: 'Longitude must be between -180 and 180.' });
  }
  if (!isWithinPortugal(lat, lng)) {
    return res.status(400).json({ error: 'Bathrooms must be within Portugal.' });
  }

  const stmt = `
    INSERT INTO bathroom_requests (user_id, name, description, lat, lng, status, created_at)
    VALUES (?, ?, ?, ?, ?, ?, ?)
  `;

  db.run(
    stmt,
    [req.user.id, name.trim(), description ? description.trim() : null, lat, lng, 'pending', new Date().toISOString()],
    function (err) {
      if (err) {
        console.error('Failed to create request:', err);
        return res.status(500).json({ error: 'Failed to submit request.' });
      }
    res.status(201).json({ id: this.lastID });
  });
});

app.get('/api/requests', requireAdmin, (req, res) => {
  db.all(
    `SELECT bathroom_requests.id, bathroom_requests.name, bathroom_requests.description,
            bathroom_requests.lat, bathroom_requests.lng, bathroom_requests.status,
            bathroom_requests.created_at, users.email AS requester_email
     FROM bathroom_requests
     LEFT JOIN users ON users.id = bathroom_requests.user_id
     WHERE bathroom_requests.status = 'pending'
     ORDER BY bathroom_requests.created_at DESC`,
    (err, rows) => {
      if (err) {
        console.error('Failed to fetch requests:', err);
        return res.status(500).json({ error: 'Failed to load requests.' });
      }
      res.json(rows);
    }
  );
});

app.get('/api/requests/mine', requireAuth, (req, res) => {
  db.all(
    `SELECT id, name, description, lat, lng, status, created_at
     FROM bathroom_requests
     WHERE user_id = ?
     ORDER BY created_at DESC`,
    [req.user.id],
    (err, rows) => {
      if (err) {
        console.error('Failed to fetch user requests:', err);
        return res.status(500).json({ error: 'Failed to load requests.' });
      }
      res.json(rows);
    }
  );
});

app.delete('/api/requests/:id', requireAuth, (req, res) => {
  const requestId = Number(req.params.id);
  if (!Number.isInteger(requestId)) {
    return res.status(400).json({ error: 'Invalid request id.' });
  }
  db.run(
    'DELETE FROM bathroom_requests WHERE id = ? AND user_id = ? AND status IN (?, ?)',
    [requestId, req.user.id, 'rejected', 'pending'],
    function (err) {
      if (err) {
        console.error('Failed to delete request:', err);
        return res.status(500).json({ error: 'Failed to delete request.' });
      }
      if (this.changes === 0) {
        return res.status(404).json({ error: 'Request not found.' });
      }
      res.json({ status: 'deleted' });
    }
  );
});

app.put('/api/requests/:id', requireAuth, (req, res) => {
  const requestId = Number(req.params.id);
  const { name, description, lat, lng } = req.body || {};
  if (!Number.isInteger(requestId)) {
    return res.status(400).json({ error: 'Invalid request id.' });
  }
  if (!isNonEmptyString(name, 80)) {
    return res.status(400).json({ error: 'Name is required (max 80 chars).' });
  }
  if (description !== undefined && typeof description !== 'string') {
    return res.status(400).json({ error: 'Description must be a string.' });
  }
  if (description && description.length > 240) {
    return res.status(400).json({ error: 'Description must be 240 chars or less.' });
  }
  if (!isFiniteNumber(lat) || lat < -90 || lat > 90) {
    return res.status(400).json({ error: 'Latitude must be between -90 and 90.' });
  }
  if (!isFiniteNumber(lng) || lng < -180 || lng > 180) {
    return res.status(400).json({ error: 'Longitude must be between -180 and 180.' });
  }
  if (!isWithinPortugal(lat, lng)) {
    return res.status(400).json({ error: 'Bathrooms must be within Portugal.' });
  }

  db.run(
    `UPDATE bathroom_requests
     SET name = ?, description = ?, lat = ?, lng = ?
     WHERE id = ? AND user_id = ? AND status = ?`,
    [name.trim(), description ? description.trim() : null, lat, lng, requestId, req.user.id, 'pending'],
    function (err) {
      if (err) {
        console.error('Failed to update request:', err);
        return res.status(500).json({ error: 'Failed to update request.' });
      }
      if (this.changes === 0) {
        return res.status(404).json({ error: 'Request not found.' });
      }
      res.json({ status: 'updated' });
    }
  );
});

app.post('/api/requests/:id/approve', requireAdmin, (req, res) => {
  const requestId = Number(req.params.id);
  if (!Number.isInteger(requestId)) {
    return res.status(400).json({ error: 'Invalid request id.' });
  }

  db.get('SELECT * FROM bathroom_requests WHERE id = ? AND status = ?', [requestId, 'pending'], (err, row) => {
    if (err) {
      console.error('Failed to load request:', err);
      return res.status(500).json({ error: 'Failed to approve request.' });
    }
    if (!row) {
      return res.status(404).json({ error: 'Request not found.' });
    }
    if (!isWithinPortugal(row.lat, row.lng)) {
      return res.status(400).json({ error: 'Request is outside Portugal bounds.' });
    }

    db.run(
      `INSERT INTO bathrooms (name, description, lat, lng, created_at)
       VALUES (?, ?, ?, ?, ?)`,
      [row.name, row.description, row.lat, row.lng, new Date().toISOString()],
      function (insertErr) {
        if (insertErr) {
          console.error('Failed to create bathroom from request:', insertErr);
          return res.status(500).json({ error: 'Failed to approve request.' });
        }

        db.run('UPDATE bathroom_requests SET status = ? WHERE id = ?', ['approved', requestId], (updateErr) => {
          if (updateErr) {
            console.error('Failed to update request status:', updateErr);
            return res.status(500).json({ error: 'Failed to approve request.' });
          }
          res.status(201).json({ id: this.lastID });
        });
      }
    );
  });
});

app.post('/api/requests/:id/reject', requireAdmin, (req, res) => {
  const requestId = Number(req.params.id);
  if (!Number.isInteger(requestId)) {
    return res.status(400).json({ error: 'Invalid request id.' });
  }

  db.run(
    'UPDATE bathroom_requests SET status = ? WHERE id = ? AND status = ?',
    ['rejected', requestId, 'pending'],
    function (err) {
      if (err) {
        console.error('Failed to reject request:', err);
        return res.status(500).json({ error: 'Failed to reject request.' });
      }
      if (this.changes === 0) {
        return res.status(404).json({ error: 'Request not found.' });
      }
      res.json({ status: 'rejected' });
    }
  );
});

app.delete('/api/bathrooms/:id', requireAdmin, (req, res) => {
  const bathroomId = Number(req.params.id);
  if (!Number.isInteger(bathroomId)) {
    return res.status(400).json({ error: 'Invalid bathroom id.' });
  }

  db.serialize(() => {
    db.run('DELETE FROM ratings WHERE bathroom_id = ?', [bathroomId], (err) => {
      if (err) {
        console.error('Failed to delete ratings:', err);
        return res.status(500).json({ error: 'Failed to delete bathroom.' });
      }
      db.run('DELETE FROM bathrooms WHERE id = ?', [bathroomId], function (deleteErr) {
        if (deleteErr) {
          console.error('Failed to delete bathroom:', deleteErr);
          return res.status(500).json({ error: 'Failed to delete bathroom.' });
        }
        if (this.changes === 0) {
          return res.status(404).json({ error: 'Bathroom not found.' });
        }
        res.json({ status: 'deleted' });
      });
    });
  });
});

app.put('/api/admin/bathrooms/:id', requireAdmin, (req, res) => {
  const bathroomId = Number(req.params.id);
  const { name, description } = req.body || {};
  if (!Number.isInteger(bathroomId)) {
    return res.status(400).json({ error: 'Invalid bathroom id.' });
  }
  if (name !== undefined && !isNonEmptyString(name, 80)) {
    return res.status(400).json({ error: 'Name must be 80 chars or less.' });
  }
  if (description !== undefined && typeof description !== 'string') {
    return res.status(400).json({ error: 'Description must be a string.' });
  }
  if (description && description.length > 240) {
    return res.status(400).json({ error: 'Description must be 240 chars or less.' });
  }

  db.run(
    'UPDATE bathrooms SET name = COALESCE(?, name), description = COALESCE(?, description) WHERE id = ?',
    [name ? name.trim() : null, description !== undefined ? description.trim() : null, bathroomId],
    function (err) {
      if (err) {
        console.error('Failed to update bathroom:', err);
        return res.status(500).json({ error: 'Failed to update bathroom.' });
      }
      if (this.changes === 0) {
        return res.status(404).json({ error: 'Bathroom not found.' });
      }
      res.json({ status: 'updated' });
    }
  );
});

app.post('/api/bathrooms/:id/ratings', requireAuth, (req, res) => {
  const bathroomId = Number(req.params.id);
  const { rating, comment } = req.body || {};

  if (!Number.isInteger(bathroomId)) {
    return res.status(400).json({ error: 'Invalid bathroom id.' });
  }
  if (!Number.isInteger(rating) || rating < 1 || rating > 5) {
    return res.status(400).json({ error: 'Rating must be an integer between 1 and 5.' });
  }
  if (comment !== undefined && typeof comment !== 'string') {
    return res.status(400).json({ error: 'Comment must be a string.' });
  }
  if (comment && comment.length > 240) {
    return res.status(400).json({ error: 'Comment must be 240 chars or less.' });
  }

  db.get('SELECT id FROM bathrooms WHERE id = ?', [bathroomId], (err, row) => {
    if (err) {
      console.error('Failed to validate bathroom:', err);
      return res.status(500).json({ error: 'Failed to add rating.' });
    }
    if (!row) {
      return res.status(404).json({ error: 'Bathroom not found.' });
    }

    const stmt = `
      INSERT INTO ratings (bathroom_id, user_id, rating, comment, created_at)
      VALUES (?, ?, ?, ?, ?)
    `;

    db.run(
      stmt,
      [bathroomId, req.user.id, rating, comment ? comment.trim() : null, new Date().toISOString()],
      function (err) {
      if (err) {
        console.error('Failed to create rating:', err);
        return res.status(500).json({ error: 'Failed to add rating.' });
      }
      res.status(201).json({ id: this.lastID });
    });
  });
});

app.put('/api/ratings/:id', requireAuth, (req, res) => {
  const ratingId = Number(req.params.id);
  const { rating, comment } = req.body || {};
  if (!Number.isInteger(ratingId)) {
    return res.status(400).json({ error: 'Invalid rating id.' });
  }
  if (!Number.isInteger(rating) || rating < 1 || rating > 5) {
    return res.status(400).json({ error: 'Rating must be an integer between 1 and 5.' });
  }
  if (comment !== undefined && typeof comment !== 'string') {
    return res.status(400).json({ error: 'Comment must be a string.' });
  }
  if (comment && comment.length > 240) {
    return res.status(400).json({ error: 'Comment must be 240 chars or less.' });
  }

  db.get('SELECT user_id FROM ratings WHERE id = ?', [ratingId], (err, row) => {
    if (err) {
      console.error('Failed to load rating:', err);
      return res.status(500).json({ error: 'Failed to update rating.' });
    }
    if (!row) {
      return res.status(404).json({ error: 'Rating not found.' });
    }
    if (row.user_id !== req.user.id) {
      return res.status(403).json({ error: 'Cannot edit this rating.' });
    }
    db.run(
      'UPDATE ratings SET rating = ?, comment = ? WHERE id = ?',
      [rating, comment ? comment.trim() : null, ratingId],
      function (updateErr) {
        if (updateErr) {
          console.error('Failed to update rating:', updateErr);
          return res.status(500).json({ error: 'Failed to update rating.' });
        }
        res.json({ status: 'updated' });
      }
    );
  });
});

app.use(express.static(path.join(__dirname, 'public')));

app.use((err, req, res, next) => {
  console.error('Unexpected error:', err);
  res.status(500).json({ error: 'Unexpected server error.' });
});

app.listen(PORT, () => {
  console.log(`Bathroom map app running at http://localhost:${PORT}`);
});
