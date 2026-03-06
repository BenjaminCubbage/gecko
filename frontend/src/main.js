import { createApp } from 'vue';

import { 
    rovingTabIndexHomeIndex,
    rovingTabIndexContainer,
    rovingTabIndexItem
} from './directives/rovingTabIndex.js';

import './style.css';
import App from './App.vue';

createApp(App)
    .directive('roving-container', rovingTabIndexContainer)
    .directive('roving-item',      rovingTabIndexItem)
    .directive('roving-home',      rovingTabIndexHomeIndex)
    .mount('#app');