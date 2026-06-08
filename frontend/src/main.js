import { createApp } from 'vue';

import {
    rovingTabIndexHomeIndex,
    rovingTabIndexContainer,
    rovingTabIndexItem
} from './directives/rovingTabIndex.js';

import {
    clickedOutside,
    clickedOutsideExcept
} from './directives/clickedOutside.js';

import './style.css';
import App from './App.vue';

createApp(App)
    .directive('roving-container',        rovingTabIndexContainer)
    .directive('roving-item',             rovingTabIndexItem)
    .directive('roving-home',             rovingTabIndexHomeIndex)
    .directive('clicked-outside',         clickedOutside)
    .directive('clicked-outside-except',  clickedOutsideExcept)
    .mount('#app');