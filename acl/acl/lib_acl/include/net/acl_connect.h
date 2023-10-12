#ifndef	ACL_CONNECT_INCLUDE_H
#define	ACL_CONNECT_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "../stdlib/acl_define.h"

#ifdef  ACL_UNIX
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif  

/* in acl_sane_connect.c */
/**
 * Զ�����ӷ�����
 * @param sock {ACL_SOCKET} �׽��֣���UNIXƽ̨�»����������׽���
 * @param sa {const struct sockaddr*} ������������ַ
 * @param len {socklen_t} sa �ĵ�ַ����
 * @return {int} 0: ���ӳɹ�; -1: ����ʧ��
 */
ACL_API int acl_sane_connect(ACL_SOCKET sock, const struct sockaddr * sa,
		socklen_t len);

/* in acl_timed_connect.c */

/**
 * ����ʱʱ���Զ�����ӷ�����
 * @param fd {ACL_SOCKET} �׽��֣���UNIXƽ̨�»����������׽���
 * @param sa {const struct sockaddr*} ������������ַ
 * @param len {socklen_t} sa �ĵ�ַ����
 * @param timeout {int} ���ӳ�ʱʱ��(�뼶)
 * @return {int} 0: ���ӳɹ�; -1: ����ʧ��
 */
ACL_API int acl_timed_connect(ACL_SOCKET fd, const struct sockaddr * sa,
		socklen_t len, int timeout);

/**
 * ����ʱʱ���Զ�����ӷ�����
 * @param fd {ACL_SOCKET} �׽��֣���UNIXƽ̨�»����������׽���
 * @param sa {const struct sockaddr*} ������������ַ
 * @param len {socklen_t} sa �ĵ�ַ����
 * @param timeout {int} ���ӳ�ʱʱ��(���뼶)
 * @return {int} 0: ���ӳɹ�; -1: ����ʧ��
 */
ACL_API int acl_timed_connect_ms(ACL_SOCKET fd, const struct sockaddr * sa,
		socklen_t len, int timeout);

/* in acl_inet_connect.c */

/**
 * Զ�����������������ַ
 * @param addr {const char*} Զ�̷������ļ�����ַ���磺192.168.0.1|80, �����
 *  Ҫ�󶨱��صĵ�ַ�����ʽΪ: remote_addr@local_ip,
 *  ��: www.sina.com|80@60.28.250.199
 * @param blocking {int} ����ģʽ���Ƿ�����ģʽ, ACL_BLOCKING �� ACL_NON_BLOCKING
 * @param timeout {int} ���ӳ�ʱʱ�䣬��� blocking Ϊ ACL_NON_BLOCKING ���ֵ��������
 * @return {ACL_SOCKET} ������� ACL_SOCKET_INVALID ��ʾ����ʧ�� 
 */
ACL_API ACL_SOCKET acl_inet_connect(const char *addr, int blocking, int timeout);

/**
 * Զ�����������������ַ
 * @param addr {const char*} Զ�̷������ļ�����ַ���磺192.168.0.1|80��
 *  �������ж��������ַ����ͨ��ĳ��ָ���������ӷ�����ʱ�ĵ�ַ��ʽ��
 *  remote_ip|remote_port@local_ip���磺211.150.111.12|80@192.168.1.1
 * @param blocking {int} ����ģʽ���Ƿ�����ģʽ, ACL_BLOCKING �� ACL_NON_BLOCKING
 * @param timeout {int} ���ӳ�ʱʱ��(�뼶)����� blocking Ϊ ACL_NON_BLOCKING ���ֵ��������
 * @param h_error {int*} ������ʧ��ʱ�洢ʧ��ԭ������
 * @return {ACL_SOCKET} ������� ACL_SOCKET_INVALID ��ʾ����ʧ�� 
 */
ACL_API ACL_SOCKET acl_inet_connect_ex(const char *addr, int blocking,
		int timeout, int *h_error);

/**
 * Զ�����������������ַ
 * @param addr {const char*} Զ�̷������ļ�����ַ���磺192.168.0.1|80��
 *  �������ж��������ַ����ͨ��ĳ��ָ���������ӷ�����ʱ�ĵ�ַ��ʽ��
 *  remote_ip|remote_port@local_ip���磺211.150.111.12|80@192.168.1.1
 * @param blocking {int} ����ģʽ���Ƿ�����ģʽ, ACL_BLOCKING �� ACL_NON_BLOCKING
 * @param timeout {int} ���ӳ�ʱʱ��(���뼶)����� blocking Ϊ ACL_NON_BLOCKING ���ֵ��������
 * @param h_error {int*} ������ʧ��ʱ�洢ʧ��ԭ������
 * @return {ACL_SOCKET} ������� ACL_SOCKET_INVALID ��ʾ����ʧ��
 */
ACL_API ACL_SOCKET acl_inet_timed_connect(const char *addr, int blocking,
		int timeout, int *h_error);

#ifdef	ACL_UNIX

/* in acl_unix_connect.c */

/**
 * ���Ӽ������׽��ַ�����
 * @param addr {const char*} ���������������׽���ȫ·��, ��: /tmp/test.sock��
 *  ��� Linux ƽ̨���������ĸΪ @ ���ڲ��Զ����ó��� UNIX ���׽ӿڽ�������
 * @param blocking {int} ����ģʽ���Ƿ�����ģʽ, ACL_BLOCKING �� ACL_NON_BLOCKING
 * @param timeout {int} ���ӳ�ʱʱ�䣬��� blocking Ϊ ACL_NON_BLOCKING ���ֵ��������
 * @return {ACL_SOCKET} ������� ACL_SOCKET_INVALID ��ʾ����ʧ�� 
 */
ACL_API ACL_SOCKET acl_unix_connect(const char *addr, int blocking, int timeout);

/* in acl_stream_connect.c */
#ifdef SUNOS5
ACL_API int acl_stream_connect(const char *path, int blocking, int timeout);
#endif

#endif

#if defined(_WIN32) || defined(_WIN64)
typedef int (WINAPI *acl_connect_fn)(SOCKET, const struct sockaddr*, socklen_t);
#else
typedef int (*acl_connect_fn)(int, const struct sockaddr*, socklen_t);
#endif

ACL_API void acl_set_connect(acl_connect_fn fn);

#ifdef	__cplusplus
}
#endif

#endif