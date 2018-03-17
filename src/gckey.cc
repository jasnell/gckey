#include "node_api.h"
#include <napi-macros.h>
#include <stdio.h>

#ifdef _WIN32
#include <stdlib.h>
#endif

#define NAPI_RETVAL_NOTHING  // Intentionally blank #define

#define GET_AND_THROW_LAST_ERROR(env)                                    \
  do {                                                                   \
    const napi_extended_error_info *error_info;                          \
    napi_get_last_error_info((env), &error_info);                        \
    bool is_pending;                                                     \
    napi_is_exception_pending((env), &is_pending);                       \
    /* If an exception is already pending, don't rethrow it */           \
    if (!is_pending) {                                                   \
      const char* error_message = error_info->error_message != NULL ?    \
        error_info->error_message :                                      \
        "empty error message";                                           \
      napi_throw_error((env), NULL, error_message);                      \
    }                                                                    \
  } while (0)

#define NAPI_CALL_BASE(env, the_call, ret_val)                           \
  do {                                                                   \
    if ((the_call) != napi_ok) {                                         \
      GET_AND_THROW_LAST_ERROR((env));                                   \
      return ret_val;                                                    \
    }                                                                    \
  } while (0)

#define NAPI_CALL(env, the_call)                                         \
  NAPI_CALL_BASE(env, the_call, NULL)

#define NAPI_CALL_RETURN_VOID(env, the_call)                             \
  NAPI_CALL_BASE(env, the_call, NAPI_RETVAL_NOTHING)

class GCKey {
 public:
  static void Init(napi_env env, napi_value exports) {
    napi_value cons;
    NAPI_CALL_RETURN_VOID(env, napi_define_class(
        env, "GCKey", -1, New, nullptr, 0, nullptr, &cons));
    NAPI_CALL_RETURN_VOID(
        env, napi_create_reference(env, cons, 1, &constructor));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, exports, "GCKey", cons));

    napi_value buffer;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_external_arraybuffer(
            env, &counter, sizeof(uint32_t), nullptr, nullptr, &buffer));
    NAPI_CALL_RETURN_VOID(
        env, napi_set_named_property(env, exports, "GCCounter", buffer));
  }

  static void Destructor(napi_env env, void* object, void*) {
    GCKey* obj = static_cast<GCKey*>(object);
    delete obj;
  }

 private:
  explicit GCKey() : env_(nullptr), wrapper_(nullptr) {}
  ~GCKey() {
    napi_delete_reference(env_, wrapper_);
    counter[0]++;
  }

  static napi_value New(napi_env env, napi_callback_info info) {
    napi_value new_target;
    NAPI_CALL(env, napi_get_new_target(env, info, &new_target));
    bool is_constructor = (new_target != nullptr);

    size_t argc = 0;
    napi_value _this;
    NAPI_CALL(
        env, napi_get_cb_info(env, info, &argc, nullptr, &_this, nullptr));

    if (is_constructor) {
      GCKey* obj = new GCKey();
      obj->env_ = env;
      NAPI_CALL(env, napi_wrap(env,
                              _this,
                              obj,
                              GCKey::Destructor,
                              nullptr,
                              &obj->wrapper_));

      return _this;
    }

    napi_value cons;
    NAPI_CALL(env, napi_get_reference_value(env, constructor, &cons));

    napi_value instance;
    NAPI_CALL(env, napi_new_instance(env, cons, 0, nullptr, &instance));

    return instance;
  }


  static napi_ref constructor;
  static uint32_t counter[1];
  napi_env env_;
  napi_ref wrapper_;
};

napi_ref GCKey::constructor;
uint32_t GCKey::counter[1] = { 0 };

napi_value Init(napi_env env, napi_value exports) {
  GCKey::Init(env, exports);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
