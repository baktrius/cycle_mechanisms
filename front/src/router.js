import { createRouter, createWebHashHistory } from 'vue-router';
import RunsPanel from './components/RunsPanel.vue';
import DocumentationPanel from './components/DocumentationPanel.vue';

const routes = [
    { path: '/', redirect: '/docs/index.md' },
    { path: '/runs', name: 'runs', component: RunsPanel },
    { path: '/docs/:docId?', name: 'docs', component: DocumentationPanel, props: true },
];

const router = createRouter({
    history: createWebHashHistory(),
    routes,
});

export default router;
