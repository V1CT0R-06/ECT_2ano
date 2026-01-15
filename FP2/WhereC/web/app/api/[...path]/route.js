const API_BASE = process.env.NEXT_PUBLIC_API_BASE || "http://localhost:3001";

const forward = async (request, context) => {
  const params = await context.params;
  const path = params?.path || [];
  const url = `${API_BASE}/api/${path.join("/")}`;
  const headers = new Headers(request.headers);
  headers.delete("host");

  const init = {
    method: request.method,
    headers,
  };

  if (request.method !== "GET" && request.method !== "HEAD") {
    init.body = await request.text();
  }

  const response = await fetch(url, init);
  const body = await response.arrayBuffer();

  return new Response(body, {
    status: response.status,
    headers: response.headers,
  });
};

export const GET = forward;
export const POST = forward;
export const PUT = forward;
export const DELETE = forward;
export const OPTIONS = forward;
