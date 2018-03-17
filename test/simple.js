const tape = require('tape')
const assert = require('assert')
const { GCKey, getGCCount } = require('../')

tape('simple', function (t) {
  const wm = new WeakMap()
  {
    let n = { m: new GCKey() }  // m will garbage collect when n is
    wm.set(n, 1)
  }
  gc()
  setImmediate(() => {
    gc()
    assert(getGCCount(), 1)
    t.end()
  });
})
