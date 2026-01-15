const loginForm = document.getElementById('loginForm');
const loginEmail = document.getElementById('loginEmail');
const loginPassword = document.getElementById('loginPassword');
const registerForm = document.getElementById('registerForm');
const registerEmail = document.getElementById('registerEmail');
const registerPassword = document.getElementById('registerPassword');
const accountStatus = document.getElementById('accountStatus');
const logoutButton = document.getElementById('logoutButton');
const adminLink = document.getElementById('adminLink');
const themeToggle = document.getElementById('themeToggle');

let authToken = localStorage.getItem('wherec-token') || '';
let currentUser = null;

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
    accountStatus.textContent = `Signed in as ${currentUser.email}`;
    logoutButton.classList.remove('hidden');
  } else {
    accountStatus.textContent = 'Not signed in.';
    logoutButton.classList.add('hidden');
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

loginForm.addEventListener('submit', async (event) => {
  event.preventDefault();
  accountStatus.textContent = '';
  const emailValue = loginEmail.value;
  const passwordValue = loginPassword.value;
  loginEmail.value = '';
  loginPassword.value = '';
  try {
    const result = await fetchJSON('/api/login', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ email: emailValue, password: passwordValue })
    });
    authToken = result.token;
    localStorage.setItem('wherec-token', authToken);
    setUser(result.user);
    alert('Signed in successfully.');
    window.location.href = 'index.html';
  } catch (error) {
    accountStatus.textContent = error.message;
  }
});

registerForm.addEventListener('submit', async (event) => {
  event.preventDefault();
  accountStatus.textContent = '';
  const emailValue = registerEmail.value;
  const passwordValue = registerPassword.value;
  registerEmail.value = '';
  registerPassword.value = '';
  try {
    await fetchJSON('/api/register', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ email: emailValue, password: passwordValue })
    });
    const result = await fetchJSON('/api/login', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ email: emailValue, password: passwordValue })
    });
    authToken = result.token;
    localStorage.setItem('wherec-token', authToken);
    setUser(result.user);
    alert('Account created and signed in.');
    window.location.href = 'index.html';
  } catch (error) {
    accountStatus.textContent = error.message;
  }
});

logoutButton.addEventListener('click', async () => {
  accountStatus.textContent = '';
  try {
    if (authToken) {
      await authFetchJSON('/api/logout', { method: 'POST' });
    }
  } catch (error) {
    accountStatus.textContent = error.message;
  } finally {
    authToken = '';
    localStorage.removeItem('wherec-token');
    setUser(null);
  }
});

initAuth();
