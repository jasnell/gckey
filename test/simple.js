const tape = require('tape')
const assert = require('assert')
const { GCKey, getGCCount } = require('../')

const count = 10

tape('simple', function (t) {
  const wm = new WeakMap()
  {
    for (let a = 0; a < count; a++) {
      let n = { m: new GCKey() }  // m will garbage collect when n is
      wm.set(n, 1)
    }
  }
  gc()
  setImmediate(() => {
    gc()
    assert(getGCCount(), count)
    t.end()
  });
})
