<template>
  <div class="mx-auto mt-16 max-w-[1220px] px-4">
    <div class="flex flex-col items-center gap-8">
      <div class="text-center text-slate-700">
        <svg id="Layer_1" data-name="Layer 1" class="mx-auto h-24 w-24" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
          <g id="Stockholm-icons-_-Home-_-Deer" data-name="Stockholm-icons-/-Home-/-Deer">
            <rect id="bound" fill="none" width="24" height="24" />
            <path id="Combined-Shape" style="fill: currentColor !important" opacity="0.3" isolation="isolate" d="M22,8.19a1,1,0,0,1-.47.67l-5,3A1,1,0,0,1,16,12H8a1,1,0,0,1-.51-.14l-5-3A1,1,0,0,1,2,8.19l-1-5a1,1,0,0,1,2-.4L3.61,6,6.29,3.29A1,1,0,0,1,7.71,4.71L4.61,7.8,8.28,10h7.44l3.67-2.2-3.1-3.09a1,1,0,0,1,1.42-1.42L20.39,6,21,2.8a1,1,0,0,1,2,.4Zm-6.93.7A1,1,0,0,1,16,7.11l3,1.5A1,1,0,0,1,19,10.36l-2.5,1.5A1,1,0,0,1,16,12H8a1,1,0,0,1-.51-.14L5,10.36A1,1,0,0,1,5,8.64l2.5-1.5a1,1,0,1,1,1,1.72L7.44,9.5l.84.5h7.44l.7-.42Z" />
            <path id="Rectangle-192" style="fill: currentColor !important" d="M9.86,10h4.28A2,2,0,0,1,16,12.68l-2.68,7.38A1.43,1.43,0,0,1,12,21h0a1.43,1.43,0,0,1-1.34-.94L8,12.68A2,2,0,0,1,9.86,10Z" />
          </g>
        </svg>
        <div class="mt-3 text-base font-semibold tracking-[0.2em] text-slate-500">ESP-DASH</div>
      </div>

      <div class="w-full max-w-md rounded-[1.6rem] bg-white px-6 py-6 shadow-[0_0_50px_rgba(232,232,232,0.9)]">
        <header class="flex items-center gap-3 text-slate-700">
          <svg class="h-6 w-6 text-[#335EEA]" viewBox="0 0 24 24" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
            <g id="Stockholm-icons-/-Code-/-Lock-circle" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
              <rect id="bound" x="0" y="0" width="24" height="24"></rect>
              <circle id="Oval-5" fill="currentColor" opacity="0.3" cx="12" cy="12" r="10"></circle>
              <path d="M14.5,11 C15.0522847,11 15.5,11.4477153 15.5,12 L15.5,15 C15.5,15.5522847 15.0522847,16 14.5,16 L9.5,16 C8.94771525,16 8.5,15.5522847 8.5,15 L8.5,12 C8.5,11.4477153 8.94771525,11 9.5,11 L9.5,10.5 C9.5,9.11928813 10.6192881,8 12,8 C13.3807119,8 14.5,9.11928813 14.5,10.5 L14.5,11 Z M12,9 C11.1715729,9 10.5,9.67157288 10.5,10.5 L10.5,11 L13.5,11 L13.5,10.5 C13.5,9.67157288 12.8284271,9 12,9 Z" id="Combined-Shape" fill="currentColor"></path>
            </g>
          </svg>
          <p class="text-lg font-semibold">Locked</p>
        </header>

        <div class="mt-5">
          <input
            class="w-full rounded-xl border border-slate-200 bg-slate-50 px-4 py-3 text-slate-500 outline-none"
            type="password"
            placeholder="Key"
            disabled
          />
        </div>

        <button
          type="button"
          class="mt-5 inline-flex w-full items-center justify-center rounded-xl bg-[#335EEA] px-4 py-3 text-sm font-semibold text-white opacity-60"
          disabled
        >
          Unlock
        </button>
      </div>
    </div>
  </div>
</template>
