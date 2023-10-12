#pragma once
#include "../acl_cpp_define.hpp"
#include "../connpool/connect_manager.hpp"

namespace acl
{

class sslbase_conf;

/**
 * HTTP �ͻ����������ӳع�����
 */
class ACL_CPP_API http_request_manager : public acl::connect_manager
{
public:
	http_request_manager();
	virtual ~http_request_manager();

	/**
	 * ���ñ��������� SSL �Ŀͻ���ģʽ
	 * @param ssl_conf {sslbase_conf*}
	 */
	void set_ssl(sslbase_conf* ssl_conf);

protected:
	/**
	 * ���ി�麯���������������ӳض��󣬸ú������غ��ɻ����������ӳص�
	 * �������Ӽ����� IO �ĳ�ʱʱ��
	 * @param addr {const char*} ������������ַ����ʽ��ip:port
	 * @param count {size_t} ���ӳصĴ�С���ƣ�����ֵΪ 0 ʱ��û������
	 * @param idx {size_t} �����ӳض����ڼ����е��±�λ��(�� 0 ��ʼ)
	 * @return {connect_pool*} ���ش��������ӳض���
	 */
	connect_pool* create_pool(const char* addr, size_t count, size_t idx);

private:
	sslbase_conf* ssl_conf_;
};

} // namespace acl