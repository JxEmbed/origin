#ifndef __PASSWORD_H
#define __PASSWORD_H

#include "data.h"

typedef struct
{
	u8 admin_pass[8];  //管理员密码
	u8 opera_pass[8];  //操作员密码
	u8 maint_pass[8];  //维护员密码
}PasswordStruct;

extern PasswordStruct Password_Par;

#endif




