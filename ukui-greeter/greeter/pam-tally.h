/*
 * Copyright (C) 2018 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
**/
#ifndef PAM_TALLY_H
#define PAM_TALLY_H

#include <time.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHM_TALLY "/shm_tally"
struct _pam_tally {
    int deny;               //失败次数上限
    int unlock_time;        //普通用户失败次数达到上限后，多少秒之后才能解锁
    int root_unlock_time;   //root用户失败次数达到上限后，多少秒之后才能解锁
    int failed;             //当前失败的次数
    time_t lock_start_time; //失败次数达到上限后，开始计时
};
typedef struct _pam_tally pam_tally;

int pam_tally_init();
int pam_tally_add_failed();
int pam_tally_clear_failed();
int pam_tally_falure_is_out();
int pam_tally_deny();
int pam_tally_failed_count();
int pam_tally_unlock_time();
int pam_tally_is_enbled();
int pam_tally_is_canUnlock();
int pam_tally_unlock_time_left(uid_t userID, int *fail_cnt, int *left_time, int *deny, int *fail_time, int *unlock_time);
int pam_tally_root_unlock_time_left(uid_t userID, int *fail_cnt, int *left_time, int *deny);

#ifdef __cplusplus
}
#endif

#endif // PAM_TALLY_H
