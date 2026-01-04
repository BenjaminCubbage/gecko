import { Delete_FriendRequest } from './Delete_FriendRequest.js'
import { Delete_Friend } from './Delete_Friend.js'
import { Get_DevicesStatus} from './Get_DevicesStatus.js';
import { Get_FriendRequests } from './Get_FriendRequests.js';
import { Get_Friends } from './Get_Friends.js';
import { Get_LatestImage } from './Get_LatestImage.js';
import { Get_UserByUsername } from './Get_UserByUsername.js';
import { Get_UsersDevices } from './Get_UsersDevices.js';
import { Get_UsersMe } from './Get_UsersMe.js';
import { Get_XSRF } from './Get_XSRF.js';
import { Patch_ChangeUsername } from './Patch_ChangeUsername.js';
import { Post_FriendRequestAccept } from './Post_FriendRequestAccept.js';
import { Post_FriendRequestCreate } from './Post_FriendRequestCreate.js';
import { Post_LogOut } from './Post_LogOut.js';
import { Post_SharedImage } from './Post_SharedImage.js';

export const Dispatch = {
    Delete_Friend:            (...args) => new Delete_Friend(...args),
    Delete_FriendRequest:     (...args) => new Delete_FriendRequest(...args),
    Get_DevicesStatus:        (...args) => new Get_DevicesStatus(...args),
    Get_FriendRequests:       (...args) => new Get_FriendRequests(...args),
    Get_Friends:              (...args) => new Get_Friends(...args),
    Get_LatestImage:          (...args) => new Get_LatestImage(...args),
    Get_UserByUsername:       (...args) => new Get_UserByUsername(...args),
    Get_UsersDevices:         (...args) => new Get_UsersDevices(...args),
    Get_UsersMe:              (...args) => new Get_UsersMe(...args),
    Get_XSRF:                 (...args) => new Get_XSRF(...args),
    Patch_ChangeUsername:     (...args) => new Patch_ChangeUsername(...args),
    Post_FriendRequestAccept: (...args) => new Post_FriendRequestAccept(...args),
    Post_FriendRequestCreate: (...args) => new Post_FriendRequestCreate(...args),
    Post_LogOut:              (...args) => new Post_LogOut(...args),
    Post_SharedImage:         (...args) => new Post_SharedImage(...args)
};