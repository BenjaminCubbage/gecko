import {
    createApp,
    watch
} from 'vue';

import {
    rovingTabIndexHomeIndex,
    rovingTabIndexContainer,
    rovingTabIndexItem
} from './directives/rovingTabIndex.js';

import {
    clickedOutside,
    clickedOutsideExcept
} from './directives/clickedOutside.js';

import {
    autoHighlight
} from './directives/autoHighlight.js';

import {
    autoFocus
} from './directives/autoFocus.js';

import { 
    interactiveAttrs
} from './directives/interactiveAttrs.js';

import { DevicesStore }  from './stores/devicesStore.js';
import { FriendsStore }  from './stores/friendsStore.js';
import { SessionStore }  from './stores/sessionStore.js';
import { Keys }          from './core/di/keys.js';

import './style.css';
import App from './App.vue';

const session  = new SessionStore();
const friends  = new FriendsStore();
const devices  = new DevicesStore();

(async () => {
    await session.requestResync();
    await friends.requestResync(session);
    await devices.requestResync(session, friends);
})();

const app = createApp(App)
    .directive('roving-container',       rovingTabIndexContainer)
    .directive('roving-item',            rovingTabIndexItem)
    .directive('roving-home',            rovingTabIndexHomeIndex)
    .directive('clicked-outside',        clickedOutside)
    .directive('clicked-outside-except', clickedOutsideExcept)
    .directive('auto-highlight',         autoHighlight)
    .directive('auto-focus',             autoFocus)
    .directive('interactive-attrs',      interactiveAttrs)
    .provide(Keys.DevicesStore,  devices)
    .provide(Keys.SessionStore,  session)
    .provide(Keys.FriendsStore,  friends);

/*
    Resync when friends / session changes
*/

watch(session.state, (newState, oldState) => {
    if ((newState === 'loggedout' && oldState === 'ready') ||
        (oldState === 'loggedout' && newState === 'ready')) {
        friends.requestResync(session);
        devices.requestResync(session, friends);
    }
});

watch(friends.activeFriends, newFriends => {
    if (devices.state.value === 'ready')
        devices.requestUpsertUserIDs(newFriends.map(f => f.user.userID));
});

app.mount('body');