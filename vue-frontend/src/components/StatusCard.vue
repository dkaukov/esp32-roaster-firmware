<template>
  <div class="relative rounded-[1.6rem] bg-white px-5 py-3 shadow-[0_0_50px_rgba(232,232,232,0.9)] md:px-6 md:py-3.5">
    <span
      class="absolute right-4 top-4 h-2.5 w-2.5 rounded-full transition-opacity duration-150"
      :class="activity ? 'bg-[#335eea] opacity-60' : 'opacity-0'"
    ></span>

    <div class="flex items-center gap-4 md:gap-5">
      <div :class="iconWrapperClasses">
        <svg width="24" height="24" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg">
          <g fill="none" fill-rule="evenodd">
            <path d="M0 0h24v24H0z"></path>
            <circle fill="currentColor" opacity=".3" cx="12" cy="12" r="10"></circle>
            <path
              v-if="statusIcon === 'success'"
              d="M16.769 7.818a1 1 0 011.462 1.364l-7 7.5a1 1 0 01-1.382.077l-3.5-3a1 1 0 011.302-1.518l2.772 2.376 6.346-6.8z"
              fill="currentColor"
            ></path>
            <path
              v-else-if="statusIcon === 'heater'"
              d="M7.4 6.5c-.55.95-.55 2.1 0 3.05.47.82.47 1.78 0 2.6-.18.31-.07.71.24.89.32.18.72.08.9-.24.7-1.22.7-2.68 0-3.9-.32-.56-.32-1.23 0-1.8.18-.31.08-.71-.24-.89-.31-.18-.71-.07-.9.24Zm4 0c-.55.95-.55 2.1 0 3.05.47.82.47 1.78 0 2.6-.18.31-.07.71.24.89.32.18.72.08.9-.24.7-1.22.7-2.68 0-3.9-.32-.56-.32-1.23 0-1.8.18-.31.08-.71-.24-.89-.31-.18-.71-.07-.9.24Zm4 0c-.55.95-.55 2.1 0 3.05.47.82.47 1.78 0 2.6-.18.31-.07.71.24.89.32.18.72.08.9-.24.7-1.22.7-2.68 0-3.9-.32-.56-.32-1.23 0-1.8.18-.31.08-.71-.24-.89-.31-.18-.71-.07-.9.24ZM6.5 16.25c0-.41.34-.75.75-.75h9.5c.41 0 .75.34.75.75s-.34.75-.75.75h-9.5a.75.75 0 0 1-.75-.75Z"
              fill="currentColor"
            ></path>
            <path
              v-else-if="statusIcon === 'danger'"
              d="M12.036 10.621l2.828-2.828a1 1 0 011.414 1.414l-2.828 2.829 2.828 2.828a1 1 0 01-1.414 1.414l-2.828-2.828-2.829 2.828a1 1 0 11-1.414-1.414l2.828-2.828-2.828-2.829a1 1 0 011.414-1.414l2.829 2.828z"
              fill="currentColor"
            ></path>
            <g v-else-if="statusIcon === 'warning'" fill="currentColor">
              <rect x="11" y="7" width="2" height="8" rx="1"></rect>
              <rect x="11" y="16" width="2" height="2" rx="1"></rect>
            </g>
            <g v-else fill="currentColor">
              <rect x="11" y="10" width="2" height="7" rx="1"></rect>
              <rect x="11" y="7" width="2" height="2" rx="1"></rect>
            </g>
          </g>
        </svg>
      </div>

      <div class="flex min-w-0 flex-1 flex-col justify-center gap-3 md:flex-row md:items-end md:justify-between md:gap-6">
        <div class="min-w-0">
          <p class="m-0 text-sm font-medium leading-none text-slate-400 md:text-[0.95rem]">{{ card.name }}</p>
          <h2 class="m-0 mt-2 text-2xl font-semibold leading-none tracking-[-0.02em] text-slate-800 md:text-[2.1rem]">{{ card.value }}</h2>
          <div v-if="card.flags && card.flags.length" class="mt-2 flex flex-wrap gap-2">
            <span
              v-for="flag in card.flags"
              :key="flag.label"
              class="inline-flex items-center rounded-full px-2.5 py-1 text-xs font-semibold"
              :class="flagClasses(flag.tone)"
            >
              {{ flag.label }}
            </span>
          </div>
        </div>

        <div v-if="card.progress" class="w-full md:max-w-[42rem] md:flex-1">
          <div class="mb-1 flex items-center justify-between text-xs font-semibold text-slate-400">
            <span>{{ card.progress.label }}</span>
            <span>{{ card.progress.value }}%</span>
          </div>
          <div class="h-2.5 overflow-hidden rounded-full bg-slate-100">
            <div
              class="h-full rounded-full transition-[width] duration-200 ease-out"
              :class="progressBarClasses(card.progress.tone)"
              :style="{ width: `${clampedProgress(card.progress.value)}%` }"
            ></div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  props: ["card"],

  data() {
    return {
      activity: true,
    };
  },

  computed: {
    statusIcon() {
      return this.card.icon || this.card.symbol;
    },

    iconWrapperClasses() {
      const map = {
        success: "flex h-12 w-12 items-center justify-center rounded-2xl bg-emerald-100 text-emerald-500",
        danger: "flex h-12 w-12 items-center justify-center rounded-2xl bg-rose-100 text-rose-500",
        warning: "flex h-12 w-12 items-center justify-center rounded-2xl bg-amber-100 text-amber-500",
      };

      return map[this.card.symbol] || "flex h-12 w-12 items-center justify-center rounded-2xl bg-slate-700 text-white";
    },
  },

  methods: {
    flagClasses(tone) {
      const map = {
        danger: "bg-rose-100 text-rose-700",
        warning: "bg-amber-100 text-amber-700",
        success: "bg-emerald-100 text-emerald-700",
      };

      return map[tone] || "bg-slate-100 text-slate-700";
    },

    progressBarClasses(tone) {
      const map = {
        danger: "bg-[#335eea]",
        warning: "bg-amber-500",
        success: "bg-emerald-500",
      };

      return map[tone] || "bg-slate-500";
    },

    clampedProgress(value) {
      const numericValue = Number(value);
      if (!Number.isFinite(numericValue)) {
        return 0;
      }

      return Math.max(0, Math.min(100, numericValue));
    },
  },

  watch: {
    "card.value": function () {
      this.activity = true;
      setTimeout(() => {
        this.activity = false;
      }, 100);
    },
    "card.symbol": function () {
      this.activity = true;
      setTimeout(() => {
        this.activity = false;
      }, 100);
    },
    "card.icon": function () {
      this.activity = true;
      setTimeout(() => {
        this.activity = false;
      }, 100);
    },
    "card.flags": {
      handler() {
        this.activity = true;
        setTimeout(() => {
          this.activity = false;
        }, 100);
      },
      deep: true,
    },
  },

  mounted() {
    setTimeout(() => {
      this.activity = false;
    }, 500);
  },
};
</script>
