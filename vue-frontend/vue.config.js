const WebpackShellPlugin = require('webpack-shell-plugin-next');
const BundleAnalyzerPlugin = require('webpack-bundle-analyzer').BundleAnalyzerPlugin;
const path = require('path');

module.exports = {
  chainWebpack: (config) => {
    config.resolve.alias.set('vue', '@vue/compat')
    config.optimization.delete('splitChunks')

    config.module
      .rule('vue')
      .use('vue-loader')
      .tap((options) => {
        return {
          ...options,
          compilerOptions: {
            compatConfig: {
              MODE: 2
            }
          }
        }
      })
  },
  pluginOptions: {
    'style-resources-loader': {
      preProcessor: 'scss',
      patterns: []
    }
  },

  outputDir: undefined,
  assetsDir: undefined,
  runtimeCompiler: undefined,
  productionSourceMap: undefined,
  parallel: undefined,
  css: { extract: false },
  filenameHashing: false,
  configureWebpack: {
      plugins: [
        new BundleAnalyzerPlugin({ analyzerMode: (process.env.NODE_ENV !== "production")?'disabled':'static'}),
        new WebpackShellPlugin({
            onBuildEnd: (process.env.NODE_ENV == 'production')?['node finalize.js && echo Finalized Webpage for Production Mode']:['echo Skipping Finalizing Vuejs Webpage in Development Mode']
        })
      ],
      resolve:{
        alias:{
          moment: 'moment/src/moment'
        }
      }
    }
}
