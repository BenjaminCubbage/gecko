import { HttpRequest } from './HttpRequest.js';

class Get_XSRF extends HttpRequest {
  constructor() {
    super('GET', '/auth/xsrf');
  }
}

export { Get_XSRF };