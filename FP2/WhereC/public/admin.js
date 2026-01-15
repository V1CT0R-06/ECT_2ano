const adminLoginForm = document.getElementById('adminLoginForm');
const adminEmail = document.getElementById('adminEmail');
const adminPassword = document.getElementById('adminPassword');
const adminStatus = document.getElementById('adminStatus');
const adminLogout = document.getElementById('adminLogout');
const requestList = document.getElementById('requestList');
const bathroomList = document.getElementById('bathroomList');
const reviewList = document.getElementById('reviewList');
const userList = document.getElementById('userList');
const themeToggle = document.getElementById('themeToggle');
const reviewEditor = document.getElementById('reviewEditor');
const reviewEditForm = document.getElementById('reviewEditForm');
const reviewRating = document.getElementById('reviewRating');
const reviewComment = document.getElementById('reviewComment');
const reviewCancel = document.getElementById('reviewCancel');
const reviewEditStatus = document.getElementById('reviewEditStatus');
const bathroomEditor = document.getElementById('bathroomEditor');
const bathroomEditForm = document.getElementById('bathroomEditForm');
const bathroomName = document.getElementById('bathroomName');
const bathroomDescription = document.getElementById('bathroomDescription');
const bathroomCancel = document.getElementById('bathroomCancel');
const bathroomEditStatus = document.getElementById('bathroomEditStatus');

let authToken = localStorage.getItem('wherec-token') || '';
let currentUser = null;
let editingReviewId = null;
let editingBathroomId = null;

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
  if (currentUser) {
    adminStatus.textContent = `Signed in as ${currentUser.email}`;
    adminLogout.classList.remove('hidden');
  } else {
    adminStatus.textContent = 'Not signed in.';
    adminLogout.classList.add('hidden');
  }
};

const applyTheme = (theme) => {
  document.body.dataset.theme = theme;
  themeToggle.checked = theme === 'dark';
  localStorage.setItem('wherec-theme', theme);
};

const initialTheme = localStorage.getItem('wherec-theme') || 'light';
applyTheme(initialTheme);

themeToggle.addEventListener('click', () => {
  const nextTheme = themeToggle.checked ? 'dark' : 'light';
  applyTheme(nextTheme);
});

const ensureAdmin = () => {
  if (!currentUser || !currentUser.is_admin) {
    requestList.textContent = 'Admin access required.';
    bathroomList.textContent = 'Admin access required.';
    reviewList.textContent = 'Admin access required.';
    userList.textContent = 'Admin access required.';
    return false;
  }
  return true;
};

const loadRequests = async () => {
  requestList.innerHTML = '';
  if (!ensureAdmin()) return;
  try {
    const requests = await authFetchJSON('/api/requests');
    if (!requests.length) {
      requestList.textContent = 'No pending requests.';
      return;
    }
    requests.forEach((request) => {
      const item = createEl('div', 'card');
      const title = createEl('p', 'card-title', request.name);
      const requester = request.requester_email || 'Unknown requester';
      const meta = createEl(
        'p',
        'card-meta',
        `${requester} · ${request.lat.toFixed(4)}, ${request.lng.toFixed(4)}`
      );
      const approve = createEl('button', 'primary', 'Approve');
      approve.addEventListener('click', async () => {
        try {
          await authFetchJSON(`/api/requests/${request.id}/approve`, { method: 'POST' });
          await loadRequests();
          await loadBathrooms();
        } catch (error) {
          alert(error.message);
        }
      });
      item.appendChild(title);
      item.appendChild(meta);
      item.appendChild(approve);
      requestList.appendChild(item);
    });
  } catch (error) {
    requestList.textContent = error.message;
  }
};

const loadBathrooms = async () => {
  bathroomList.innerHTML = '';
  if (!ensureAdmin()) return;
  try {
    const bathrooms = await authFetchJSON('/api/bathrooms');
    if (!bathrooms.length) {
      bathroomList.textContent = 'No bathrooms available.';
      return;
    }
    bathrooms.forEach((bathroom) => {
      const item = createEl('div', 'card');
      const title = createEl('p', 'card-title', bathroom.name);
      const meta = createEl(
        'p',
        'card-meta',
        `${bathroom.avg_rating ? bathroom.avg_rating.toFixed(1) : '0.0'} ★ · ${bathroom.rating_count} ratings`
      );
      const editButton = createEl('button', 'ghost', 'Edit bathroom');
      const removeButton = createEl('button', 'ghost', 'Remove bathroom');
      editButton.addEventListener('click', async () => {
        editingBathroomId = bathroom.id;
        bathroomName.value = bathroom.name || '';
        bathroomDescription.value = bathroom.description || '';
        bathroomEditStatus.textContent = '';
        bathroomEditor.classList.remove('hidden');
      });
      removeButton.addEventListener('click', async () => {
        if (!confirm(`Remove ${bathroom.name}?`)) return;
        try {
          await authFetchJSON(`/api/bathrooms/${bathroom.id}`, { method: 'DELETE' });
          await loadBathrooms();
          await loadRequests();
        } catch (error) {
          alert(error.message);
        }
      });
      item.appendChild(title);
      item.appendChild(meta);
      item.appendChild(editButton);
      item.appendChild(removeButton);
      bathroomList.appendChild(item);
    });
  } catch (error) {
    bathroomList.textContent = error.message;
  }
};

const loadReviews = async () => {
  reviewList.innerHTML = '';
  if (!ensureAdmin()) return;
  try {
    const reviews = await authFetchJSON('/api/admin/ratings');
    if (!reviews.length) {
      reviewList.textContent = 'No reviews available.';
      return;
    }
    reviews.forEach((review) => {
      const item = createEl('div', 'card');
      const title = createEl('p', 'card-title', review.bathroom_name);
      const meta = createEl('p', 'card-meta', `${review.rating} ★ · ${review.comment || 'No comment'}`);
      const actions = createEl('div', 'review-actions');
      const editButton = createEl('button', 'ghost', 'Edit review');
      const removeButton = createEl('button', 'ghost', 'Remove review');
      editButton.addEventListener('click', async () => {
        editingReviewId = review.id;
        reviewRating.value = review.rating.toString();
        reviewComment.value = review.comment || '';
        reviewEditStatus.textContent = '';
        reviewEditor.classList.remove('hidden');
      });
      removeButton.addEventListener('click', async () => {
        if (!confirm('Remove this review?')) return;
        try {
          await authFetchJSON(`/api/admin/ratings/${review.id}`, { method: 'DELETE' });
          await loadReviews();
          await loadBathrooms();
        } catch (error) {
          alert(error.message);
        }
      });
      item.appendChild(title);
      item.appendChild(meta);
      actions.appendChild(editButton);
      actions.appendChild(removeButton);
      item.appendChild(actions);
      reviewList.appendChild(item);
    });
  } catch (error) {
    reviewList.textContent = error.message;
  }
};

const loadUsers = async () => {
  userList.innerHTML = '';
  if (!ensureAdmin()) return;
  try {
    const users = await authFetchJSON('/api/admin/users');
    if (!users.length) {
      userList.textContent = 'No users found.';
      return;
    }
    users.forEach((user) => {
      const item = createEl('div', 'card');
      const title = createEl('p', 'card-title', user.email);
      const meta = createEl('p', 'card-meta', user.is_admin ? 'Admin account' : 'Standard account');
      const passwordInput = createEl('input');
      passwordInput.type = 'password';
      passwordInput.placeholder = 'New password';
      const resetButton = createEl('button', 'ghost', 'Change password');
      resetButton.addEventListener('click', async () => {
        const newPassword = passwordInput.value.trim();
        passwordInput.value = '';
        if (!newPassword) {
          alert('Password required.');
          return;
        }
        try {
          await authFetchJSON(`/api/admin/users/${user.id}/password`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ password: newPassword })
          });
          alert('Password updated.');
        } catch (error) {
          alert(error.message);
        }
      });

      item.appendChild(title);
      item.appendChild(meta);
      item.appendChild(passwordInput);
      item.appendChild(resetButton);

      if (!user.is_admin) {
        const removeButton = createEl('button', 'ghost', 'Remove account');
        removeButton.addEventListener('click', async () => {
          if (!confirm(`Remove ${user.email}?`)) return;
          try {
            await authFetchJSON(`/api/admin/users/${user.id}`, { method: 'DELETE' });
            await loadUsers();
            await loadRequests();
          } catch (error) {
            alert(error.message);
          }
        });
        item.appendChild(removeButton);
      }

      userList.appendChild(item);
    });
  } catch (error) {
    userList.textContent = error.message;
  }
};

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

adminLoginForm.addEventListener('submit', async (event) => {
  event.preventDefault();
  adminStatus.textContent = '';
  const emailValue = adminEmail.value;
  const passwordValue = adminPassword.value;
  adminEmail.value = '';
  adminPassword.value = '';
  try {
    const result = await fetchJSON('/api/login', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ email: emailValue, password: passwordValue })
    });
    authToken = result.token;
    localStorage.setItem('wherec-token', authToken);
    setUser(result.user);
    if (!result.user.is_admin) {
      adminStatus.textContent = 'This account is not an admin.';
      return;
    }
    await loadRequests();
    await loadBathrooms();
    await loadReviews();
    await loadUsers();
  } catch (error) {
    adminStatus.textContent = error.message;
  }
});

adminLogout.addEventListener('click', async () => {
  adminStatus.textContent = '';
  try {
    if (authToken) {
      await authFetchJSON('/api/logout', { method: 'POST' });
    }
  } catch (error) {
    adminStatus.textContent = error.message;
  } finally {
    authToken = '';
    localStorage.removeItem('wherec-token');
    setUser(null);
    requestList.innerHTML = '';
    bathroomList.innerHTML = '';
    reviewList.innerHTML = '';
    userList.innerHTML = '';
    reviewEditor.classList.add('hidden');
    editingReviewId = null;
    bathroomEditor.classList.add('hidden');
    editingBathroomId = null;
  }
});

bathroomEditForm.addEventListener('submit', async (event) => {
  event.preventDefault();
  if (!editingBathroomId) {
    bathroomEditStatus.textContent = 'No bathroom selected.';
    return;
  }
  const nameValue = bathroomName.value.trim();
  const descriptionValue = bathroomDescription.value.trim();
  if (!nameValue && !descriptionValue) {
    bathroomEditStatus.textContent = 'Provide a name or description.';
    return;
  }
  try {
    await authFetchJSON(`/api/admin/bathrooms/${editingBathroomId}`, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        name: nameValue || undefined,
        description: descriptionValue || undefined
      })
    });
    bathroomEditStatus.textContent = 'Bathroom updated.';
    editingBathroomId = null;
    bathroomEditor.classList.add('hidden');
    await loadBathrooms();
  } catch (error) {
    bathroomEditStatus.textContent = error.message;
  }
});

bathroomCancel.addEventListener('click', () => {
  editingBathroomId = null;
  bathroomEditStatus.textContent = '';
  bathroomEditor.classList.add('hidden');
});

reviewEditForm.addEventListener('submit', async (event) => {
  event.preventDefault();
  if (!editingReviewId) {
    reviewEditStatus.textContent = 'No review selected.';
    return;
  }
  const ratingValue = Number(reviewRating.value);
  const commentValue = reviewComment.value;
  try {
    await authFetchJSON(`/api/admin/ratings/${editingReviewId}`, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ rating: ratingValue, comment: commentValue })
    });
    reviewEditStatus.textContent = 'Review updated.';
    editingReviewId = null;
    reviewEditor.classList.add('hidden');
    await loadReviews();
  } catch (error) {
    reviewEditStatus.textContent = error.message;
  }
});

reviewCancel.addEventListener('click', () => {
  editingReviewId = null;
  reviewEditStatus.textContent = '';
  reviewEditor.classList.add('hidden');
});

initAuth().then(() => {
  loadRequests();
  loadBathrooms();
  loadReviews();
  loadUsers();
});
