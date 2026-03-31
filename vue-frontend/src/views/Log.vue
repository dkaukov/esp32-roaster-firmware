<template>
  <div class="mx-auto max-w-[1220px]">
    <div class="log-viewer">
      <pre>{{ displayLog }}</pre>
    </div>
  </div>
</template>

<script>
export default {
  name: "log",

  props: ["log"],

  computed: {
    displayLog() {
      return String(this.log.lines || "")
        .replace(/\r\n/g, "\n")
        .replace(/\n{2,}/g, "\n");
    },
  },
};
</script>

<style>
.log-viewer {
  height: calc(100vh - 11rem);
  min-height: calc(100vh - 11rem);
  max-height: calc(100vh - 11rem);
  overflow: auto;
  padding: 0.5rem 0.75rem;
  border: 1px solid #0f172a;
  border-radius: 4px;
  background: #0a0f1d !important;
  color: #d6deeb !important;
  font-size: 13px;
  line-height: 1.15;
  font-family: "SFMono-Regular", "Menlo", "Monaco", "Consolas", "Liberation Mono", monospace;
}

.log-viewer pre {
  margin: 0;
  padding: 0;
  border: 0;
  background: transparent !important;
  color: inherit !important;
  white-space: pre;
  word-break: normal;
  min-width: max-content;
}
</style>
