/* Capstone Disassembly Engine */
/* By Travis Finkenauer <tmfinken@gmail.com>, 2018 */

#ifndef CS_POWERPC_MODULE_H
#define CS_POWERPC_MODULE_H

#include "../../utils.h"

cs_err PPC_global_init(cs_struct *ud);
cs_err PPC_option(cs_struct *handle, cs_opt_type type, size_t value);

#endif
