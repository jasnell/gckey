const path = require('path')
const {
  GCCounter,
  GCKey
} = require('node-gyp-build')(path.join(__dirname))

const counter = new Int32Array(GCCounter)

function getGCCount () {
  return counter[0]
}

module.exports = {
  GCKey,
  getGCCount
}
