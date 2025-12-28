import { Get_DevicesStatus} from './Get_DevicesStatus.js';
import { Get_LatestImage } from './Get_LatestImage.js';
import { Get_UsersDevices } from './Get_UsersDevices.js';
import { Get_UsersMe } from './Get_UsersMe.js';
import { Get_XSRF } from './Get_XSRF.js';
import { Patch_ChangeUsername } from './Patch_ChangeUsername.js';
import { Post_LogOut } from './Post_LogOut.js';
import { Post_SharedImage } from './Post_SharedImage.js';

export const Dispatch = {
    Get_DevicesStatus:    (...args) => new Get_DevicesStatus(...args),
    Get_LatestImage:      (...args) => new Get_LatestImage(...args),
    Get_UsersDevices:     (...args) => new Get_UsersDevices(...args),
    Get_UsersMe:          (...args) => new Get_UsersMe(...args),
    Get_XSRF:             (...args) => new Get_XSRF(...args),
    Patch_ChangeUsername: (...args) => new Patch_ChangeUsername(...args),
    Post_LogOut:          (...args) => new Post_LogOut(...args),
    Post_SharedImage:     (...args) => new Post_SharedImage(...args)
};