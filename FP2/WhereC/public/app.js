const portugalBounds = L.latLngBounds(
  [36.8, -9.6],
  [42.3, -6.0]
);

const map = L.map('map', {
  zoomControl: false,
  maxBounds: portugalBounds,
  maxBoundsViscosity: 0.8
}).setView([39.6, -8.1], 7);
const lightTiles = L.tileLayer('https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}{r}.png', {
  subdomains: 'abcd',
  maxZoom: 19,
  attribution: '&copy; OpenStreetMap &copy; CARTO'
});
const darkTiles = L.tileLayer('https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}{r}.png', {
  subdomains: 'abcd',
  maxZoom: 19,
  attribution: '&copy; OpenStreetMap &copy; CARTO'
});
L.control.zoom({ position: 'bottomright' }).addTo(map);

const state = {
  bathrooms: [],
  markers: new Map(),
  selectedId: null
};

const bathroomList = document.getElementById('bathroomList');
const bathroomCount = document.getElementById('bathroomCount');
const averageRating = document.getElementById('averageRating');
const detailContent = document.getElementById('detailContent');
const formMessage = document.getElementById('formMessage');
const bathroomForm = document.getElementById('bathroomForm');
const useLocation = document.getElementById('useLocation');
const themeToggle = document.getElementById('themeToggle');
const adminLink = document.getElementById('adminLink');
const authHint = document.getElementById('authHint');

let authToken = localStorage.getItem('wherec-token') || '';
let currentUser = null;

const applyTheme = (theme) => {
  document.body.dataset.theme = theme;
  themeToggle.checked = theme === 'dark';
  if (theme === 'dark') {
    map.addLayer(darkTiles);
    map.removeLayer(lightTiles);
  } else {
    map.addLayer(lightTiles);
    map.removeLayer(darkTiles);
  }
  localStorage.setItem('wherec-theme', theme);
};

const initialTheme = localStorage.getItem('wherec-theme') || 'light';
applyTheme(initialTheme);

const initAuth = async () => {
  if (!authToken) {
    setUser(null);
    return;
  }
  try {
    const user = await authFetchJSON('/api/me');
    setUser(user);
  } catch (error) {
    authToken = '';
    localStorage.removeItem('wherec-token');
    setUser(null);
  }
};

const createEl = (tag, className, text) => {
  const el = document.createElement(tag);
  if (className) el.className = className;
  if (text !== undefined) el.textContent = text;
  return el;
};

const fetchJSON = async (url, options) => {
  const response = await fetch(url, options);
  const data = await response.json();
  if (!response.ok) {
    throw new Error(data.error || 'Request failed');
  }
  return data;
};

const authFetchJSON = async (url, options = {}) => {
  if (!authToken) {
    throw new Error('Sign in required.');
  }
  const headers = Object.assign({}, options.headers, {
    Authorization: `Bearer ${authToken}`
  });
  return fetchJSON(url, { ...options, headers });
};

const setUser = (user) => {
  currentUser = user;
  if (authHint) {
    authHint.textContent = currentUser ? `Signed in as ${currentUser.email}` : 'Sign in on the account page to request bathrooms.';
  }
  if (!adminLink) {
    return;
  }
  if (currentUser && currentUser.is_admin) {
    adminLink.classList.remove('hidden');
    adminLink.hidden = false;
  } else {
    adminLink.classList.add('hidden');
    adminLink.hidden = true;
  }
};

const updateStats = () => {
  bathroomCount.textContent = state.bathrooms.length.toString();
  const totalRatings = state.bathrooms.reduce((sum, b) => sum + (b.rating_count || 0), 0);
  const weighted = state.bathrooms.reduce(
    (sum, b) => sum + (b.avg_rating || 0) * (b.rating_count || 0),
    0
  );
  const avg = totalRatings ? (weighted / totalRatings) : 0;
  averageRating.textContent = avg.toFixed(1);
};

const renderList = () => {
  bathroomList.innerHTML = '';
  state.bathrooms.forEach((bathroom) => {
    const card = createEl('div', 'card');
    if (bathroom.id === state.selectedId) {
      card.classList.add('active');
    }
    const title = createEl('p', 'card-title', bathroom.name);
    const meta = createEl(
      'p',
      'card-meta',
      `${bathroom.avg_rating ? bathroom.avg_rating.toFixed(1) : '0.0'} ★ · ${bathroom.rating_count} ratings`
    );
    card.appendChild(title);
    card.appendChild(meta);
    card.addEventListener('click', () => selectBathroom(bathroom.id));
    bathroomList.appendChild(card);
  });
};

const renderMarkers = () => {
  state.markers.forEach((marker) => map.removeLayer(marker));
  state.markers.clear();

  const markerIcon = L.divIcon({
    className: 'bathroom-marker',
    html: '🚻',
    iconSize: [28, 28],
    iconAnchor: [14, 14]
  });

  state.bathrooms.forEach((bathroom) => {
    const marker = L.marker([bathroom.lat, bathroom.lng], { icon: markerIcon }).addTo(map);
    marker.on('click', () => selectBathroom(bathroom.id));
    state.markers.set(bathroom.id, marker);
  });
};

const focusMap = (bathroom) => {
  map.setView([bathroom.lat, bathroom.lng], 15, { animate: true });
};

const renderDetail = async (bathroom) => {
  detailContent.innerHTML = '';

  const header = createEl('div');
  const title = createEl('h3', null, bathroom.name);
  const description = createEl('p', 'muted', bathroom.description || 'No description provided.');
  const ratingChip = createEl(
    'span',
    'rating-chip',
    `${bathroom.avg_rating ? bathroom.avg_rating.toFixed(1) : '0.0'} ★ (${bathroom.rating_count})`
  );
  header.appendChild(title);
  header.appendChild(description);
  header.appendChild(ratingChip);

  const ratingList = createEl('div', 'rating-list');
  ratingList.textContent = 'Loading ratings...';

  const ratingForm = createEl('form', 'form');
  ratingForm.innerHTML = `
    <label>
      Rating
      <select name="rating" required>
        <option value="">Select</option>
        <option value="5">5 - Excellent</option>
        <option value="4">4 - Good</option>
        <option value="3">3 - Okay</option>
        <option value="2">2 - Needs work</option>
        <option value="1">1 - Avoid</option>
      </select>
    </label>
    <label>
      Comment
      <textarea name="comment" maxlength="240" placeholder="Share a quick note"></textarea>
    </label>
    <button type="submit" class="primary">Submit rating</button>
    <p class="form-message" role="status"></p>
  `;

  ratingForm.addEventListener('submit', async (event) => {
    event.preventDefault();
    if (!authToken) {
      const message = ratingForm.querySelector('.form-message');
      message.textContent = 'Sign in to leave a rating.';
      message.style.color = '#b13e2f';
      return;
    }
    const formData = new FormData(ratingForm);
    const rating = Number(formData.get('rating'));
    const comment = formData.get('comment');
    const message = ratingForm.querySelector('.form-message');
    message.textContent = '';

    try {
      await authFetchJSON(`/api/bathrooms/${bathroom.id}/ratings`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ rating, comment })
      });
      message.textContent = 'Thanks for the rating!';
      message.style.color = 'var(--blue)';
      await loadBathrooms(bathroom.id);
    } catch (error) {
      message.textContent = error.message;
      message.style.color = '#b13e2f';
    }
  });

  detailContent.appendChild(header);
  detailContent.appendChild(ratingForm);
  detailContent.appendChild(ratingList);

    try {
      const ratings = authToken
        ? await authFetchJSON(`/api/bathrooms/${bathroom.id}/ratings`)
        : await fetchJSON(`/api/bathrooms/${bathroom.id}/ratings`);
      ratingList.innerHTML = '';
      if (!ratings.length) {
        ratingList.textContent = 'No ratings yet. Be the first!';
        return;
      }
      ratings.forEach((rating) => {
        const item = createEl('div', 'rating-item');
        const text = rating.comment ? `${rating.rating} ★ — ${rating.comment}` : `${rating.rating} ★`;
        item.textContent = text;
        if (rating.owned) {
          const actions = createEl('div', 'review-actions');
          const editButton = createEl('button', 'ghost', 'Edit review');
          const editBox = createEl('form', 'form hidden');
          editBox.innerHTML = `
            <label>
              Rating
              <select name="rating" required>
                <option value="5">5 - Excellent</option>
                <option value="4">4 - Good</option>
                <option value="3">3 - Okay</option>
                <option value="2">2 - Needs work</option>
                <option value="1">1 - Avoid</option>
              </select>
            </label>
            <label>
              Comment
              <textarea name="comment" maxlength="240" placeholder="Update your note"></textarea>
            </label>
            <button type="submit" class="primary">Save changes</button>
          `;
          editBox.rating.value = rating.rating.toString();
          editBox.comment.value = rating.comment || '';
          editButton.addEventListener('click', () => {
            editBox.classList.toggle('hidden');
          });
          editBox.addEventListener('submit', async (event) => {
            event.preventDefault();
            const formData = new FormData(editBox);
            const newRating = Number(formData.get('rating'));
            const newComment = formData.get('comment');
            try {
              await authFetchJSON(`/api/ratings/${rating.id}`, {
                method: 'PUT',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ rating: newRating, comment: newComment })
              });
              await renderDetail(bathroom);
            } catch (error) {
              alert(error.message);
            }
          });
          actions.appendChild(editButton);
          item.appendChild(actions);
          item.appendChild(editBox);
        }
        ratingList.appendChild(item);
      });
    } catch (error) {
      ratingList.textContent = 'Unable to load ratings.';
    }
};

const selectBathroom = async (id) => {
  state.selectedId = id;
  renderList();
  const bathroom = state.bathrooms.find((item) => item.id === id);
  if (!bathroom) return;
  focusMap(bathroom);
  await renderDetail(bathroom);
};

const loadBathrooms = async (selectId) => {
  try {
    const bathrooms = await fetchJSON('/api/bathrooms');
    state.bathrooms = bathrooms;
    updateStats();
    renderList();
    renderMarkers();

    const target = selectId || (bathrooms[0] && bathrooms[0].id);
    if (target) {
      await selectBathroom(target);
    }
  } catch (error) {
    bathroomList.textContent = 'Unable to load bathrooms.';
  }
};

map.on('click', (event) => {
  const { lat, lng } = event.latlng;
  bathroomForm.lat.value = lat.toFixed(4);
  bathroomForm.lng.value = lng.toFixed(4);
});

useLocation.addEventListener('click', () => {
  if (!navigator.geolocation) {
    formMessage.textContent = 'Geolocation not supported in this browser.';
    return;
  }
  navigator.geolocation.getCurrentPosition(
    (position) => {
      const { latitude, longitude } = position.coords;
      if (!portugalBounds.contains([latitude, longitude])) {
        formMessage.textContent = 'Location is outside Portugal.';
        return;
      }
      bathroomForm.lat.value = latitude.toFixed(4);
      bathroomForm.lng.value = longitude.toFixed(4);
      map.setView([latitude, longitude], 15, { animate: true });
    },
    () => {
      formMessage.textContent = 'Unable to access location.';
    }
  );
});

themeToggle.addEventListener('click', () => {
  const nextTheme = themeToggle.checked ? 'dark' : 'light';
  applyTheme(nextTheme);
});

bathroomForm.addEventListener('submit', async (event) => {
  event.preventDefault();
  formMessage.textContent = '';
  const formData = new FormData(bathroomForm);
  const payload = {
    name: formData.get('name'),
    description: formData.get('description'),
    lat: Number(formData.get('lat')),
    lng: Number(formData.get('lng'))
  };

  if (!authToken) {
    formMessage.textContent = 'Sign in to submit requests.';
    return;
  }

  if (!portugalBounds.contains([payload.lat, payload.lng])) {
    formMessage.textContent = 'Bathrooms must be within Portugal.';
    return;
  }

  try {
    await authFetchJSON('/api/requests', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload)
    });
    formMessage.textContent = 'Request sent for approval.';
    bathroomForm.reset();
  } catch (error) {
    formMessage.textContent = error.message;
  }
});

initAuth();
loadBathrooms();
