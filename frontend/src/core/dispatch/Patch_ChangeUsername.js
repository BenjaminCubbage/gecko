import { HttpRequest } from './HttpRequest.js';

class Patch_ChangeUsername extends HttpRequest {
  constructor(userID, newUsername) {
    const url = `/api/users/${userID}`;
    super('PATCH', url, { username: newUsername });
  }
}

export { Patch_ChangeUsername };