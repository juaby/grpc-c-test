/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#ifndef TEST_GRPC_C_CALL_OPS_H
#define TEST_GRPC_C_CALL_OPS_H

#include "../grpc_c_public.h"
#include "message.h"
#include "context.h"
#include <grpc/grpc.h>
#include <stdbool.h>

typedef GRPC_method grpc_method;
typedef struct grpc_context grpc_context;
typedef struct grpc_call_op_set grpc_call_op_set;

typedef bool (*grpc_op_filler)(grpc_op *op, const grpc_method *, grpc_context *, grpc_call_op_set *, const grpc_message message, grpc_message *response);
typedef void (*grpc_op_finisher)(grpc_context *, grpc_call_op_set *, bool *status, int max_message_size);

typedef struct grpc_op_manager {
  const grpc_op_filler fill;
  const grpc_op_finisher finish;
} grpc_op_manager;

enum { GRPC_MAX_OP_COUNT = 8 };

typedef struct grpc_call_op_set {
  const grpc_op_manager op_managers[GRPC_MAX_OP_COUNT];
  grpc_context * const context;

  /* these are used by individual operations */
  grpc_message *response;
  grpc_byte_buffer *recv_buffer;
  bool message_received;

  /* if this is true (default false), the event tagged by this call_op_set will not be emitted
   * from the completion queue wrapper. */
  bool hide_from_user;

  // used in async calls
  void *user_tag;
  bool *user_done;    // for clients reading a stream
} grpc_call_op_set;

void grpc_fill_op_from_call_set(grpc_call_op_set *set, const grpc_method *rpc_method, grpc_context *context,
                                const grpc_message message, grpc_message *response, grpc_op ops[], size_t *nops);

/* Runs post processing steps in the call op set. Returns false if something wrong happens e.g. serialization. */
bool grpc_finish_op_from_call_set(grpc_call_op_set *set, grpc_context *context);

void grpc_start_batch_from_op_set(grpc_call *call, grpc_call_op_set *set, grpc_context *context,
                                  const grpc_message message, grpc_message *response);

/* list of operations */

extern const grpc_op_manager grpc_op_send_metadata;
extern const grpc_op_manager grpc_op_recv_metadata;
extern const grpc_op_manager grpc_op_send_object;
extern const grpc_op_manager grpc_op_recv_object;
extern const grpc_op_manager grpc_op_send_close;
extern const grpc_op_manager grpc_op_recv_status;

#endif //TEST_GRPC_C_CALL_OPS_H
