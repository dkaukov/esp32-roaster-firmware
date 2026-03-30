const { gzip } = require('@gfx/zopfli');
const FS = require('fs');
const path = require('path');

const DIST_DIR = path.resolve(__dirname, './dist');

function inlineAsset(assetPath, wrap) {
  const normalizedPath = assetPath.replace(/^\.?\//, '');
  const absolutePath = path.resolve(DIST_DIR, normalizedPath);
  const fileContents = FS.readFileSync(absolutePath, 'utf8');
  return wrap(fileContents);
}

const HTML = FS.readFileSync(path.resolve(DIST_DIR, 'index.html'), 'utf8')
  .replace(
    /<link[^>]+rel="stylesheet"[^>]+href="([^"]+)"[^>]*>/g,
    (_, assetPath) => inlineAsset(assetPath, (contents) => `<style>${contents}</style>`)
  )
  .replace(
    /<script[^>]+type="module"[^>]+src="([^"]+)"[^>]*><\/script>/g,
    (_, assetPath) => inlineAsset(assetPath, (contents) => `<script type="module">${contents}</script>`)
  );

function chunkArray(myArray, chunk_size) {
  let index = 0;
  const arrayLength = myArray.length;
  const tempArray = [];
  for (index = 0; index < arrayLength; index += chunk_size) {
    let myChunk = myArray.slice(index, index + chunk_size);
    // Do something if you want with the group
    tempArray.push(myChunk);
  }
  return tempArray;
}

function addLineBreaks(buffer) {
  let data = '';
  const chunks = chunkArray(buffer, 30);
  chunks.forEach((chunk, index) => {
    data += chunk.join(',');
    if (index + 1 !== chunks.length) {
      data += ',\n';
    }
  });
  return data;
}


gzip(HTML, { numiterations: 15, blocksplittingmax: 100 }, (err, output) => {
  if (err) {
    /* eslint-disable no-console */
    return console.error(err);
    /* eslint-enable no-console */
  }

  const FILE = `#ifndef DashWebpage_h
#define DashWebpage_h
const uint32_t DASH_HTML_SIZE = ${output.length};
const uint8_t DASH_HTML[] PROGMEM = { 
${addLineBreaks(output)} 
};
#endif
`;

  FS.writeFileSync(path.resolve(__dirname, '../src/network/webpage.h'), FILE);
  /* eslint-disable no-console */
  console.log(`[COMPRESS] Compressed Build Files to webpage.h: ${ (output.length/1024).toFixed(2) }KB`);
  /* eslint-enable no-console */
});
