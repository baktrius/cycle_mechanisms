<script setup>
import { ref, onMounted, watch, nextTick, useTemplateRef } from "vue";
import { marked } from "marked";
import markedKatex from "marked-katex-extension";
import { gfmHeadingId } from "marked-gfm-heading-id";
import "katex/dist/katex.min.css";
import { useRoute } from "vue-router";

const props = defineProps({
  docId: {
    type: String,
    default: "index.md",
  },
});
const contentTop = useTemplateRef("content-top");

const route = useRoute();
const content = ref("");

const renderer = new marked.Renderer();
const originalLink = renderer.link;
renderer.link = function (...args) {
  const arg = args[0];
  // marked v5+ passes an object { href, title, text }
  // older versions pass (href, title, text)
  // We'll handle both just in case, though the previous code used args object.
  let href, title, text;
  if (typeof arg === "object" && arg !== null && !Array.isArray(arg)) {
    ({ href, title, text } = arg);
  } else {
    [href, title, text] = [arguments[0], arguments[1], arguments[2]];
  }

  if (href && href.endsWith(".md")) {
    // Transform .md links to router-compatible hash links
    // If there is an anchor like file.md#section, we need to preserve it
    // But since we are using hash routing, it gets tricky: #/docs/file.md#section
    // The router might interpret the second # as part of the hash.
    // Let's assume simple case first.
    return `<a href="#/docs/${href}" title="${title || ""}">${text}</a>`;
  }
  // Handle internal anchors within the same page or cross-page
  if (href && href.startsWith("#")) {
    // It's an anchor on the current page.
    // We want to scroll to it.
    // But standard <a href="#id"> might conflict with vue router hash mode.
    // However, usually browsers handle #id scrolling even in hash mode if implemented correctly,
    // or we might need to intercept the click.
    // For now let's leave standard behavior or adjust if needed.
  }

  console.log("Custom link renderer args:", args);
  return originalLink.apply(this, args);
};

marked.use(
  markedKatex({
    throwOnError: false,
  }),
  gfmHeadingId({ prefix: "doc-" }),
  { renderer, pedantic: false, gfm: true }
);

function handleHashScroll(hash, initial = false) {
  console.log("Handling hash scroll:", hash, initial);
  if (hash) {
    const id = "doc-" + decodeURIComponent(hash.substring(1));
    const element = document.getElementById(id);
    console.log("Scrolling to id:", id, element);
    if (element) {
      // Remove any previous highlight
      const prev = document.querySelector(".markdown-body .highlight");
      if (prev) prev.classList.remove("highlight");
      element.scrollIntoView({ behavior: "smooth" });
      // Add temporary highlight and remove it after a short delay
      element.classList.add("highlight");
    }
  } else {
    // Scroll to top if no hash
    if (contentTop.value && initial) {
      contentTop.value.scrollIntoView();
    }
  }
}

async function loadDoc(filename) {
  if (!filename) filename = "index.md";
  try {
    // Fetch from public/docs/
    const res = await fetch(`docs/${filename}`);
    if (!res.ok) throw new Error("Failed to load documentation");
    const text = await res.text();
    content.value = marked.parse(text);

    // Scroll to anchor if present
    nextTick(async () => {
      handleHashScroll(route.hash, true);
    });
  } catch (e) {
    content.value = `<p class="error">Error loading documentation: ${e.message}</p>`;
  } finally {
  }
}

watch(
  () => props.docId,
  (newVal) => {
    loadDoc(newVal);
  }
);

// Watch for hash changes to scroll to anchor
watch(() => handleHashScroll(route.hash));

onMounted(() => {
  loadDoc(props.docId);
});
</script>

<template>
  <div class="doc-panel">
    <div class="doc-content">
      <div ref="content-top"></div>
      <div v-html="content" class="markdown-body"></div>
    </div>
  </div>
</template>

<style scoped>
.doc-panel {
  display: flex;
  height: 100%;
  overflow: hidden;
}

.doc-content {
  flex-grow: 1;
  padding: 0px 20px;
  overflow-y: auto;
}

.markdown-body {
  text-align: justify;
}

/* Basic Markdown Styles */
.markdown-body :deep(h1) {
  font-size: 1.5em;
  border-bottom: 1px solid #eaecef;
  padding-bottom: 0.3em;
}
.markdown-body :deep(h2) {
  font-size: 1.3em;
  border-bottom: 1px solid #eaecef;
  padding-bottom: 0.3em;
  margin-top: 1.5em;
}
.markdown-body :deep(ul) {
  padding-left: 20px;
}
.markdown-body :deep(code) {
  background-color: rgba(27, 31, 35, 0.05);
  padding: 0.2em 0.4em;
  border-radius: 3px;
}
.markdown-body :deep(pre) {
  background-color: #f6f8fa;
  padding: 16px;
  overflow: auto;
  border-radius: 3px;
}
.markdown-body :deep(.highlight) {
  background-color: #fff9c4; /* pale yellow */
  transition: background-color 0.6s ease;
  text-decoration: underline;
}
</style>
