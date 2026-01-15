# WhereC Bathroom Maps

Bathroom discovery and rating webapp with a Next.js frontend and a secure Express + SQLite API.

## Features
- Map-based bathroom discovery (Leaflet + CARTO tiles)
- Account system for requests and admin moderation (admin dashboard in `/admin`)
- Request new bathrooms by clicking the map or using geolocation
- Admin approval flow for new bathrooms and removals
- Community ratings with comments
- Seeded bathrooms for Aveiro, Porto, and Lisbon
- Basic security hardening (Helmet, rate limiting, input validation)

## Run locally
Start the API (Express + SQLite):
```bash
npm install
PORT=3001 npm start
```

Start the Next.js frontend:
```bash
cd web
npm install
NEXT_PUBLIC_API_BASE="http://localhost:3001" npm run dev
```
Then open `http://localhost:3000`.

## Accounts
Use `/auth` to sign in or create an account. Requests require a signed-in user.

## Admin mode
Set an admin account before starting the server:
```bash
ADMIN_EMAIL="admin@example.com" ADMIN_PASSWORD="your-secret-password" PORT=3001 npm start
```
In the UI, sign in with the admin account and open `/admin` to approve requests, manage users, and remove bathrooms.

## Reset seed data
If you already ran the app and want the new seed data, delete `data/app.db` and restart the server.

## API
- `GET /api/bathrooms`
- `POST /api/requests`
- `GET /api/requests/mine`
- `DELETE /api/requests/:id`
- `PUT /api/requests/:id`
- `GET /api/requests` (admin)
- `POST /api/requests/:id/approve` (admin)
- `POST /api/requests/:id/reject` (admin)
- `DELETE /api/bathrooms/:id` (admin)
- `POST /api/register`
- `POST /api/login`
- `POST /api/logout`
- `GET /api/me`
- `GET /api/admin/users` (admin)
- `POST /api/admin/users/:id/password` (admin)
- `DELETE /api/admin/users/:id` (admin)
- `PUT /api/admin/bathrooms/:id` (admin)
- `GET /api/admin/ratings` (admin)
- `PUT /api/admin/ratings/:id` (admin)
- `DELETE /api/admin/ratings/:id` (admin)
- `PUT /api/admin/users/:id/role` (admin)
- `GET /api/bathrooms/:id/ratings`
- `POST /api/bathrooms/:id/ratings`
- `PUT /api/ratings/:id`


## Reset Everything and run as super-admin
Run this in terminal:

cd /home/victor/ECT_2ano/FP2/WhereC
pkill -f "node server.js" || true
pkill -f "next dev" || true
ADMIN_EMAIL="moraisvictor201106@gmail.com" ADMIN_PASSWORD="your-password" PORT=3001 npm start &
cd web
npm run dev
