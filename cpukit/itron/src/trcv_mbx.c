/*
 *  ITRON 3.0 Mailbox Manager
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/itron.h>

#include <rtems/itron/mbox.h>
#include <rtems/itron/task.h>

/*
 *  trcv_msg - Receive Message from Mailbox with Timeout
 */

ER trcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid,
  TMO     tmout
)
{
  register ITRON_Mailbox_Control *the_mailbox;
  Watchdog_Interval               interval;
  boolean                         wait;
  Objects_Locations               location;
  uint32_t                        size;

  if (!ppk_msg)
    return E_PAR;

  interval = 0;
  if ( tmout == TMO_POL ) {
    wait = FALSE;
  } else {
    wait = TRUE;
    if ( tmout != TMO_FEVR )
      interval = TOD_MILLISECONDS_TO_TICKS(tmout);
  }

  if ( wait && _ITRON_Is_in_non_task_state() )
    return E_CTX;

  the_mailbox = _ITRON_Mailbox_Get( mbxid, &location );
  switch ( location ) {
#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      return _ITRON_Mailbox_Clarify_get_id_error( mbxid );

    case OBJECTS_LOCAL:

      _CORE_message_queue_Seize(
        &the_mailbox->message_queue,
        the_mailbox->Object.id,
        ppk_msg,
        &size,
        wait,
        interval
      );
      break;
  }

  _ITRON_return_errorno(
    _ITRON_Mailbox_Translate_core_message_queue_return_code(
        _Thread_Executing->Wait.return_code ) );
}
