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
            <path v-if="card.symbol === 'success'" d="M16.769 7.818a1 1 0 011.462 1.364l-7 7.5a1 1 0 01-1.382.077l-3.5-3a1 1 0 011.302-1.518l2.772 2.376 6.346-6.8z" fill="currentColor"></path>
            <path v-else-if="card.symbol === 'danger'" d="M12.036 10.621l2.828-2.828a1 1 0 011.414 1.414l-2.828 2.829 2.828 2.828a1 1 0 01-1.414 1.414l-2.828-2.828-2.829 2.828a1 1 0 11-1.414-1.414l2.828-2.828-2.828-2.829a1 1 0 011.414-1.414l2.829 2.828z" fill="currentColor"></path>
            <g v-else-if="card.symbol === 'warning'" fill="currentColor">
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

      <div class="flex min-w-0 flex-col justify-center">
        <p class="m-0 text-sm font-medium leading-none text-slate-400 md:text-[0.95rem]">{{ card.name }}</p>
        <h2 class="m-0 mt-2 text-2xl font-semibold leading-none tracking-[-0.02em] text-slate-800 md:text-[2.1rem]">{{ card.value }}</h2>
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
    iconWrapperClasses() {
      const map = {
        success: "flex h-12 w-12 items-center justify-center rounded-2xl bg-emerald-100 text-emerald-500",
        danger: "flex h-12 w-12 items-center justify-center rounded-2xl bg-rose-100 text-rose-500",
        warning: "flex h-12 w-12 items-center justify-center rounded-2xl bg-amber-100 text-amber-500",
      };

      return map[this.card.symbol] || "flex h-12 w-12 items-center justify-center rounded-2xl bg-slate-700 text-white";
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
  },

  mounted() {
    setTimeout(() => {
      this.activity = false;
    }, 500);
  },
};
</script>
