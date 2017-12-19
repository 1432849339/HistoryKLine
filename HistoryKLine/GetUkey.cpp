#include "GetUkey.h"

chronos::SecurityMaster ukey_db;
map<std::string, int> g_market_ids =
{
	{ "SZ",  MARKET_SZA },
	{ "SH",  MARKET_SHA },
	{ "SZB", MARKET_SZB },
	{ "SHB", MARKET_SHB },
	{ "CF",  MARKET_CFE },
	{ "SHF", MARKET_SHF },
	{ "CZC", MARKET_CZC },
	{ "DCE", MARKET_DCE },
	{ "SGE", MARKET_SGE },
};

bool Init_ukdb09(const std::set<std::string>& market_set, bool use_file)
{
	Config*	config = Config::GetConfigPtr();
	if (!config->Has_Load())
	{
		cerr << "请初始化配置文件!" << endl;
		return false;
	}
	std::cout << "InitUkeyDB start use_file=" << use_file << endl;
	int count = 0;
	string dir_ = config->GetConfigByName("ukey_path");
	if (use_file)
	{
		count = ukey_db.UKOpen(dir_);
	}
	else
	{
		set<int> market_ids;
		bool market_error = false;
		if (market_set.size() == 0)
		{
			for (auto &p : g_market_ids)
			{
				market_ids.insert(p.second);
			}
		}
		else
		{
			auto it = g_market_ids.begin();
			for (auto &mk : market_set)
			{
				if ((it = g_market_ids.find(mk)) != g_market_ids.end())
				{
					market_ids.insert(it->second);
				}
				else
				{
					market_error = true;
					cerr << "Market ID is not defined market=" << mk << endl;
				}
			}
		}
		if (market_ids.size() == 0 || market_error)
		{
			cout << "Init Market ID error" << endl;
			return false;
		}
		for (auto mk : market_ids)
		{
			cout << "ukey_db market_id=" << mk << endl;
		}
		count = ukey_db.UKOpen(market_ids, config->GetConfigByName("mysql_ip"), config->GetConfigByName("mysql_name"), config->GetConfigByName("mysql_password"), config->GetConfigByName("mysql_database"), 0);
	}
	cout << "InitUkeyDB done use_file=" << use_file << ", count=" << count << endl;
	return true;
}

int64_t	GetUkey(std::string market, std::string code)
{
	std::transform(market.begin(), market.end(), market.begin(), ::toupper);
	auto it = g_market_ids.find(market);
	if (it == g_market_ids.end())
	{
		cerr << "Market ID not defined market=" << market << ", code=" << code << endl;
		return 0;
	}

	string ucode = code;
	std::transform(ucode.begin(),ucode.end(), ucode.begin(), ::toupper);
	SecurityInfo info;
	if (market == "SZ" || market == "SH")
	{
		auto ret = ukey_db.GetBaseInfo(it->second, ucode, info);
		if (ret)
		{
			auto ret = ukey_db.GetBaseInfo(it->second + 7, ucode, info);
			if (ret)
			{
				return 0;
			}
		}
	}
	else
	{
		auto ret = ukey_db.GetBaseInfo(it->second, ucode, info);
		if (ret)
		{
			return 0;
		}
	}
	return info.ukey;
}