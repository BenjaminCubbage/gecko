import { Get_UsersMe } from './Get_UsersMe.js';
import { Patch_ChangeUsername } from './Patch_ChangeUsername.js';
import { Post_LogOut } from './Post_LogOut.js';
import { Get_XSRF } from './Get_XSRF.js';

export const Dispatch = {
    Get_UsersMe:          (...args) => new Get_UsersMe(...args),
    Patch_ChangeUsername: (...args) => new Patch_ChangeUsername(...args),
    Post_LogOut:          (...args) => new Post_LogOut(...args),
    Get_XSRF:             (...args) => new Get_XSRF(...args)
};