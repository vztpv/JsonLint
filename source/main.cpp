

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>


#define ENTER '\n' // cf) std::endl

// ClauText
#define ARRAY_QUEUE std::deque // chk?
#define VECTOR std::vector
#include <wiz/ClauText.h>
// ~ClauText

namespace Lint {
	wiz::StringBuilder builder(102400);
	bool cout_off = false; // print yes or no?
	wiz::Option opt;

	// justOne + optional_ -> no ok?
	// onemore + optional_ -> no ok?
	class Option
	{
	public:
		enum class Type_ { ANY, INT, FLOAT, QUOTED_STRING, STRING, DATETIME, DATETIME_A, DATETIME_B };

		enum class Id_ { NONE, ID, TOTAL_ID };
		enum class OneMore_ { ONEMORE, JUSTONE };
		enum class Required_ { REQUIRED, OPTIONAL_ };
		enum class EmptyUT_ { NONE, OFF, ON };

		enum class Order_ { NONE, OFF, ON }; // with global??
		enum class Multiple_ { NONE, OFF, ON };
	public:
		std::vector<Type_> type;
		Id_ id;
		OneMore_ onemore;
		EmptyUT_ empty_ut;
		Required_ required;
		std::string event_ids;

		std::string prefix;
	public:
		Option() : type(), //id(Id_::NONE), 
			//onemore(OneMore_::NONE), 
			required(Required_::REQUIRED),
			empty_ut(EmptyUT_::NONE)
		{
			//
		}
	public:
		Option & Type(Type_ type) {
			this->type.push_back(type);
			return *this;
		}
		Option & Id(Id_ id) {
			this->id = id;
			return *this;
		}
		Option & OneMore(OneMore_ onemore) {
			this->onemore = onemore;
			return *this;
		}
		Option & EmptyUT(EmptyUT_ empty_ut) {
			this->empty_ut = empty_ut;
			return *this;
		}
		Option & Required(Required_ required) {
			this->required = required;
			return *this;
		}
		Option & Event(const std::string& event_id) {
			this->event_ids += " ";
			this->event_ids += event_id;
			return *this;
		}
	public:
		// check valid function?
		bool IsValid() const {
			// to do
		}
	};


	Option OptionFrom(const std::string& option_str)
	{
		Option option;
		std::string::size_type start = 0;
		std::string::size_type find_percent = std::string::npos; // % : delimeter.

		if ((find_percent = option_str.find('%', start)) == std::string::npos)
		{
			option.prefix = option_str;
		}
		else {
			option.prefix = option_str.substr(start, find_percent - start);
		}

		while ((find_percent = option_str.find('%', start)) != std::string::npos)
		{
			std::string::size_type end_ = option_str.find('%', find_percent + 1);

			start = find_percent; // ex) abc%id?

			if (end_ == std::string::npos) {
				end_ = option_str.size();
			}

			const std::string opt = option_str.substr(start, end_ - 1 - start + 1);

			if ("%int" == opt) {
				option.Type(Option::Type_::INT);
			}
			else if ("%float" == opt) {
				option.Type(Option::Type_::FLOAT);
			}
			else if ("%quoted_string" == opt) {
				option.Type(Option::Type_::QUOTED_STRING);
			}
			else if ("%string" == opt) {
				option.Type(Option::Type_::STRING);
			}
			else if ("%datetime" == opt) {
				option.Type(Option::Type_::DATETIME);
			}
			else if ("%datetime_a" == opt) {
				option.Type(Option::Type_::DATETIME_A);
			}
			else if ("%datetime_b" == opt) {
				option.Type(Option::Type_::DATETIME_B);
			}
			else if ("%id" == opt) {
				option.Id(Option::Id_::ID);
			}
			else if ("%total_id" == opt) {
				option.Id(Option::Id_::TOTAL_ID);
			}
			else if ("%any" == opt) {
				option.Type(Option::Type_::ANY);
			}
			else if ("%one_more" == opt) { // x = { 1 2 3 4  } -> x = { %int%one_more%event_plus_test }
				option.OneMore(Option::OneMore_::ONEMORE);
			}
			else if ("%just_one" == opt) {
				option.OneMore(Option::OneMore_::JUSTONE);
			}
			else if ("%optional" == opt) {
				option.Required(Option::Required_::OPTIONAL_);
			}
			else if ("%required" == opt) {
				option.Required(Option::Required_::REQUIRED);
			}
			else if ("%can_empty_ut" == opt) {
				option.EmptyUT(Option::EmptyUT_::ON);
			}
			else if (wiz::String::startsWith(opt, "%event_")) { // size check?
				std::string event_name = wiz::String::substring(opt, 7);
				option.Event(std::move(event_name));
			}
			else {
				std::cout << "wrong option" << ENTER;
				exit(-2);
			}

			start = end_;
		}

		if (option.type.empty()) {
			option.type.push_back(Option::Type_::ANY);
		}
		return option;
	}

	// chk valid in here!
	inline bool OptionDoA(const Option& option, const std::string& str)
	{
		if (option.prefix.empty() == false &&
			option.prefix == str) {
			return true;
		}
		else if (option.prefix.empty() == false &&
			option.prefix != str) {
			return false;
		}

		long long count = option.type.size();
		auto type_list = option.type;

		while (!type_list.empty()) {
			switch (type_list.back()) {
			case Option::Type_::ANY:
				// pass
				break;
			case Option::Type_::INT:
				if (wiz::load_data::Utility::IsInteger(str)) {
					//
				}
				else {
					std::cout << str << " is not integer" << ENTER;
					count--;
				}
				break;
			case Option::Type_::FLOAT:
				if (wiz::load_data::Utility::IsDouble(str)) {
					//
				}
				else {
					std::cout << str << " is not double" << ENTER;
					count--;
				}
				break;
			case Option::Type_::DATETIME:
				if (wiz::load_data::Utility::IsDate(str)) {
					//
				}
				else {
					std::cout << str << " is not date" << ENTER;
					count--;
				}
				break;
			case Option::Type_::DATETIME_A:
				if (wiz::load_data::Utility::IsDateTimeA(str)) {
					//
				}
				else {
					std::cout << str << " is not datetime A" << ENTER;
					count--;
				}
				break;
			case Option::Type_::DATETIME_B:
				if (wiz::load_data::Utility::IsDateTimeB(str)) {
					//
				}
				else {
					std::cout << str << " is not datetime B" << ENTER;
					count--;
				}
				break;
			case Option::Type_::QUOTED_STRING:
				if (str.size() >= 2 && str[0] == str.back() && str.back() == '\"') { // cf '\''
					//
				}
				else {
					std::cout << str << " is not quoted string" << ENTER;
					count--;
				}
				break;
			case Option::Type_::STRING:
				// pass
				break;
			}
			type_list.pop_back();
		}

		return count > 0;
	}

	std::tuple<bool, Option, Option> _Check(wiz::load_data::UserType* schema_eventUT,
		const wiz::load_data::ItemType<WIZ_STRING_TYPE>& x, const wiz::load_data::ItemType<WIZ_STRING_TYPE>& y, const std::string& real_dir) //, Order?
	{
		const Option var_option = OptionFrom(x.GetName().ToString()); // name, value check - not start with % ??
		const Option val_option = OptionFrom(x.Get(0).ToString());

		// val only case, ex) A = { a b c d } , a, b, c, d `s name is empty.
		if (x.GetName().ToString().empty()) {
			if (!y.GetName().ToString().empty()) {
				//
				return{ false, var_option, val_option };
			}
		}
		else { // bug?
			if (y.GetName().ToString().empty()) {
				//
				return{ false, var_option, val_option };
			}
		}

		// option type check.
		const bool name_do = OptionDoA(var_option, y.GetName().ToString());
		const bool val_do = OptionDoA(val_option, y.Get(0).ToString());

		if (name_do && val_do) {
			// event check.
			wiz::ClauText jsonText;
			wiz::StringTokenizer tokenizer(var_option.event_ids, &builder);
			wiz::StringTokenizer tokenizer2(val_option.event_ids, &builder);
			std::string event_name;

			while (tokenizer.hasMoreTokens()) {
				event_name = tokenizer.nextToken();

				// for var // chk no start with __, no end with __ ?
				wiz::load_data::LoadData::AddData(*schema_eventUT, "/./",
					"Event = { id = __" + event_name + "__ $call = { id = " + event_name +
					" name = " + y.GetName().ToString() + " value = " + y.Get(0).ToString() +
					" is_usertype = FALSE " +
					" real_dir = " + real_dir +
					" select = NAME " +
					" input = " + y.GetName().ToString() +
					" } }",
					"TRUE", wiz::ExcuteData(), &builder);

				if ("TRUE" == jsonText.excute_module("Main = { $call = { id = __" + event_name + "__ } }", schema_eventUT, wiz::ExcuteData(), opt, 1)) {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);
				}
				else {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);

					std::cout << "jsonText is not valid1" << ENTER;
					return { false, var_option, val_option };
				}
			}
			while (tokenizer2.hasMoreTokens()) {
				event_name = tokenizer2.nextToken();
				// for val
				wiz::load_data::LoadData::AddData(*schema_eventUT, "/./",
					"Event = { id = __" + event_name + "__ $call = { id = " + event_name +
					" name = " + y.GetName().ToString() + " value = " + y.Get(0).ToString() +
					" is_usertype = FALSE " +
					" real_dir = " + real_dir +
					" select = VALUE " +
					" input = " + y.Get(0).ToString() +
					" } }",
					"TRUE", wiz::ExcuteData(), &builder);

				if ("TRUE" == jsonText.excute_module("Main = { $call = { id = __" + event_name + "__ } }", schema_eventUT, wiz::ExcuteData(), opt, 1)) {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);
				}
				else {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);

					std::cout << "jsonText is not valid2" << ENTER;
					return { false, var_option, val_option };
				}
			}
		}
		else {
			return { false, var_option, val_option };
		}

		return { true, var_option, val_option };
	}

	std::tuple<bool, Option> _Check(wiz::load_data::UserType* schema_eventUT,
		const wiz::load_data::UserType* x, const wiz::load_data::UserType* y, const std::string& real_dir // Order?
	)
	{
		Option var_option = OptionFrom(x->GetName().ToString()); // name, value check - not start with % ??

		// val only case, ex) A = { a b c d } , a, b, c, d `s name is empty.
		if (x->GetName().ToString().empty()) {
			if (!y->GetName().ToString().empty()) {
				//
				return{ false, var_option };
			}
		}
		else { // bug?
			if (y->GetName().ToString().empty()) {
				//
				return{ false, var_option };
			}
		}

		// option type check.
		const bool name_do = OptionDoA(var_option, y->GetName().ToString());

		if (name_do) {
			// event check.
			wiz::ClauText jsonText;
			wiz::StringTokenizer tokenizer(var_option.event_ids, &builder);
			std::string event_name;

			while (tokenizer.hasMoreTokens()) {
				event_name = tokenizer.nextToken();

				// for var // chk no start with __, no end with __ ?
				wiz::load_data::LoadData::AddData(*schema_eventUT, "/./",
					"Event = { id = __" + event_name + "__ $call = { id = " + event_name +
					" name = " + y->GetName().ToString() +
					" is_usertype = TRUE " +
					" real_dir = " + real_dir +
					" select = NAME " +
					" input = " + y->GetName().ToString() +
					"}  } ",
					"TRUE", wiz::ExcuteData(), &builder);

				if ("TRUE" == jsonText.excute_module("Main = { $call = { id = __" + event_name + "__ } }", schema_eventUT, wiz::ExcuteData(), opt, 1)) {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);
				}
				else {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);

					std::cout << "jsonText is not valid3" << ENTER;
					return { false, var_option };
				}
			}
		}
		else {
			return { false, var_option };
		}

		return { true, var_option };
	}

	// varaible!
	std::set<std::tuple<std::string, std::string, std::string>> check_total_id;

	bool Check(wiz::load_data::UserType* schema_eventUT, wiz::load_data::UserType* schemaUT,
		const wiz::load_data::UserType* jsontextUT, int depth, bool& log_on)
	{
		Option::Order_ order = Option::Order_::OFF;
		long long jt_itCount = 0; // for jsontextUT?
		long long jt_utCount = 0; // for jsontextUT?
		long long itCount = 0;
		long long utCount = 0;

		long multiple_flag = 0; // 0 : no multiple, 1 : multiple - only arr of item, or arr of usertype.

		// for ORDER_::OFF
		std::vector<bool> validVisit(schemaUT->GetIListSize(), false);
		std::vector<Option> varOptionVisit(schemaUT->GetIListSize()); // remove?
		std::vector<bool> mark(jsontextUT->GetItemListSize(), false); // ct_it
		std::vector<bool> mark2(jsontextUT->GetUserTypeListSize(), false); // ct_ut

		std::set<std::pair<std::string, std::string>> check_id;
		std::set<std::pair<std::string, std::string>> check_justone;


		if (schemaUT->GetItemListSize() > 0 && schemaUT->GetItemList(itCount).ToString() == "%order_on") {
			order = Option::Order_::ON;
			validVisit[itCount] = true;
			itCount++;
		}
		else if (schemaUT->GetItemListSize() > 0 && schemaUT->GetItemList(itCount).ToString() == "%order_off") {
			order = Option::Order_::OFF;
			validVisit[itCount] = true;
			itCount++;
		}

		for (long long i = itCount; i < schemaUT->GetIListSize(); ++i)
		{
			if (depth == 0) {
				check_total_id.clear();
			}

			const bool chk_ct_it = jt_itCount < jsontextUT->GetItemListSize();
			const bool chk_ct_ut = jt_utCount < jsontextUT->GetUserTypeListSize();

			if (schemaUT->IsItemList(i)) {
				if (schemaUT->GetItemList(itCount).ToString() == "%multiple_on") {
					if (order == Option::Order_::OFF) {
						std::cout << "to do %multple_on, need to %order_on!" << ENTER;
						return false;
					}
					multiple_flag = 1;
					validVisit[i] = true;
					itCount++;
					continue;
				}
				else if (schemaUT->GetItemList(itCount).ToString() == "%log_on") {
					log_on = true; 
					validVisit[i] = true;
					itCount++;
					continue;
				}
				else if (schemaUT->GetItemList(itCount).ToString() == "%log_off") {
					log_on = false;
					validVisit[i] = true;
					itCount++;
					continue;
				}

				// log
				if (log_on) {
					std::cout << ENTER << "<itemtype> ";
					std::cout << "[depth] : " << depth << " ";
					std::cout << "[~th] : " << itCount << " ";
					std::cout << "[schema] : " << schemaUT->GetItemList(itCount).ToString() << " ";
				}

				// off -> no order? : slow??
				if (order == Option::Order_::OFF) {
					bool pass = false;
					std::tuple<bool, Option, Option> temp;
					// schemaUT?
					for (long long j = 0; j < jsontextUT->GetItemListSize(); ++j) {
						if (log_on) {
							std::cout << ENTER << "\t" << "[json ~th] : " << j << " "
								<< "[jsontext] : " << jsontextUT->GetItemList(j).ToString() << ENTER;
						}

						temp = _Check(schema_eventUT, schemaUT->GetItemList(itCount), jsontextUT->GetItemList(j), wiz::load_data::LoadData::GetRealDir(jsontextUT->GetItemList(j).GetName().ToString(), jsontextUT, &builder));
						if (mark[j] == false &&
							std::get<0>(temp)
							) {
							// visit vector? chk?
							validVisit[i] = true;
							varOptionVisit[i] = std::get<1>(temp);
							mark[j] = true;

							pass = true;

							// check id, total id! 
							if (std::get<1>(temp).id == Option::Id_::ID) {
								const std::string key_1 = jsontextUT->GetItemList(j).GetName().ToString();
								const std::string key_2 = "it_name";
								const std::pair<std::string, std::string> key(key_1, key_2);

								if (check_id.find(key) == check_id.end()) {
									check_id.insert(key);
								}
								else {
									std::cout << "jsonText is not valid - ID1" << ENTER;
									return false;
								}
							}
							else if (std::get<1>(temp).id == Option::Id_::TOTAL_ID) {
								if (0 == depth) {
									std::cout << "ERROR schema is not valid1" << ENTER;
									return false;
								}
								const std::string key_1 = jsontextUT->GetItemList(j).GetName().ToString();
								const std::string key_2 = wiz::load_data::LoadData::GetRealDir(schemaUT->GetName().ToString(), schemaUT, &builder);
								const std::string key_3 = "it_name";

								std::tuple<std::string, std::string, std::string> key(key_1, key_2, key_3);

								if (check_total_id.find(key) == check_total_id.end()) {
									check_total_id.insert(key);
								}
								else {
									std::cout << "jsonText is not valid _ ID2" << ENTER;
									return false;
								}
							}
							if (std::get<2>(temp).id == Option::Id_::ID) {
								const std::string key_1 = jsontextUT->GetItemList(j).Get(0).ToString();
								const std::string key_2 = "it_value";
								const std::pair<std::string, std::string> key(key_1, key_2);

								if (check_id.find(key) == check_id.end()) {
									check_id.insert(key);
								}
								else {
									std::cout << "jsonText is not valid - ID3" << ENTER;
									return false;
								}
							}
							else if (std::get<2>(temp).id == Option::Id_::TOTAL_ID) {
								if (0 == depth) {
									std::cout << "ERROR schema is not valid2" << ENTER;
									return false;
								}

								const std::string key_1 = jsontextUT->GetItemList(j).Get(0).ToString();
								const std::string key_2 = wiz::load_data::LoadData::GetRealDir(schemaUT->GetName().ToString(), schemaUT, &builder);
								const std::string key_3 = "it_value";

								std::tuple<std::string, std::string, std::string> key(key_1, key_2, key_3);

								if (check_total_id.find(key) == check_total_id.end()) {
									check_total_id.insert(key);
								}
								else {
									std::cout << "jsonText is not valid _ ID4" << ENTER;
									return false;
								}
							}

							// check justone, (onemore)
							if (std::get<1>(temp).onemore == Option::OneMore_::JUSTONE) { // justone -> only for name! , not for value!
								if (check_justone.find(std::make_pair(jsontextUT->GetItemList(j).GetName().ToString(), std::string("it"))) != check_justone.end()) {
									std::cout << "jsonText is not valid, justone is set, but not justone. 1" << ENTER;
									return false;
								}
								else {
									check_justone.insert(std::make_pair(jsontextUT->GetItemList(j).GetName().ToString(), std::string("it")));
								}
								break;
							}
							else if (std::get<1>(temp).onemore == Option::OneMore_::ONEMORE) {
								//
							}
							else {
								break;
							}
						}
					}

					if (false == pass) {
						Option var_option = OptionFrom(schemaUT->GetItemList(itCount).GetName().ToString());

						if (var_option.required == Option::Required_::OPTIONAL_) { // optional -> only for name
							jt_itCount--;
							validVisit[i] = true;
						}
						else {
							std::cout << "jsonText is not valid4" << ENTER;
							return false;
						}
					}
				}
				else if (order == Option::Order_::ON) {
					if (!chk_ct_it) {
						break;
					}

					if (log_on) {
						std::cout << "[json ~th] : " << jt_itCount << " "
							<< "[jsontext] : " << jsontextUT->GetItemList(jt_itCount).ToString() << ENTER;
					}

					std::tuple<bool, Option, Option> temp;
					temp = _Check(schema_eventUT, schemaUT->GetItemList(itCount), jsontextUT->GetItemList(jt_itCount), wiz::load_data::LoadData::GetRealDir(jsontextUT->GetItemList(jt_itCount).GetName().ToString(), jsontextUT, &builder));

					if (std::get<0>(temp)) {
						validVisit[i] = true;

						// check id, total id!
						if (std::get<1>(temp).id == Option::Id_::ID) {
							const std::string key_1 = jsontextUT->GetItemList(jt_itCount).GetName().ToString();
							const std::string key_2 = "it_name";
							const std::pair<std::string, std::string> key(key_1, key_2);

							if (check_id.find(key) == check_id.end()) {
								check_id.insert(key);
							}
							else {
								std::cout << "jsonText is not valid - ID5" << ENTER;
								return false;
							}
						}
						else if (std::get<1>(temp).id == Option::Id_::TOTAL_ID) {
							if (0 == depth) {
								std::cout << "ERROR schema is not valid3" << ENTER;
								return false;
							}

							const std::string key_1 = jsontextUT->GetItemList(jt_itCount).GetName().ToString();
							const std::string key_2 = wiz::load_data::LoadData::GetRealDir(schemaUT->GetName().ToString(), schemaUT, &builder);
							const std::string key_3 = "it_name";

							std::tuple<std::string, std::string, std::string> key(key_1, key_2, key_3);

							if (check_total_id.find(key) == check_total_id.end()) {
								check_total_id.insert(key);
							}
							else {
								std::cout << "jsonText is not valid _ ID6" << ENTER;
								return false;
							}
						}
						if (std::get<2>(temp).id == Option::Id_::ID) {
							const std::string key_1 = jsontextUT->GetItemList(jt_itCount).Get(0).ToString();
							const std::string key_2 = "it_value";
							const std::pair<std::string, std::string> key(key_1, key_2);

							if (check_id.find(key) == check_id.end()) {
								check_id.insert(key);
							}
							else {
								std::cout << "jsonText is not valid - ID7" << ENTER;
								return false;
							}
						}
						else if (std::get<2>(temp).id == Option::Id_::TOTAL_ID) {
							if (0 == depth) {
								std::cout << "ERROR schema is not valid4" << ENTER; // use id!
								return false;
							}

							const std::string key_1 = jsontextUT->GetItemList(jt_itCount).Get(0).ToString();
							const std::string key_2 = wiz::load_data::LoadData::GetRealDir(schemaUT->GetName().ToString(), schemaUT, &builder);
							const std::string key_3 = "it_value";

							std::tuple<std::string, std::string, std::string> key(key_1, key_2, key_3);

							if (check_total_id.find(key) == check_total_id.end()) {
								check_total_id.insert(key);
							}
							else {
								std::cout << "jsonText is not valid _ ID8" << ENTER;
								return false;
							}
						}

						// check justone, (onemore)
						if (std::get<1>(temp).onemore == Option::OneMore_::JUSTONE) { // justone -> only for name! , not for value!
							if (check_justone.find(std::make_pair(jsontextUT->GetItemList(jt_itCount).GetName().ToString(), std::string("it"))) != check_justone.end()) {
								std::cout << "jsonText is not valid, justone is set, but not justone. 2" << ENTER;
								return false;
							}
							else {
								check_justone.insert(std::make_pair(jsontextUT->GetItemList(jt_itCount).GetName().ToString(), std::string("it")));
							}
						}
						else if (std::get<1>(temp).onemore == Option::OneMore_::ONEMORE) {
							if (jt_itCount < jsontextUT->GetItemListSize() - 1
								&& itCount < schemaUT->GetItemListSize()
								&& jsontextUT->GetItemList(jt_itCount).GetName() == jsontextUT->GetItemList(jt_itCount + 1).GetName()
								) {
								if (1 != multiple_flag) {
									--i; itCount--;
								}
							}
						}
						else {
							//
						}
					}
					else if (std::get<1>(temp).required == Option::Required_::OPTIONAL_) {
						jt_itCount--;
						validVisit[i] = true;
					}
					else {
						std::cout << "jsonText is not valid6" << ENTER;
						return false;
					}


					if (1 == multiple_flag) {
						itCount--; i--;
					}
				}
				jt_itCount++;
				itCount++;
			}
			else { // usertype
				// log
				if (log_on && chk_ct_ut) {
					std::cout << ENTER << "<usertype> ";
					std::cout << "[depth] : " << depth << " ";
					std::cout << "[~th] : " << utCount << " ";
					std::cout << "[schema] : " << schemaUT->GetUserTypeList(utCount)->GetName().ToString() << " ";
				}

				// off -> no order? : slow??
				if (order == Option::Order_::OFF) {
					bool pass = false;
					bool use_onemore = false;
					std::tuple<bool, Option> temp;

					for (long long j = 0; j < jsontextUT->GetUserTypeListSize(); ++j) {
						if (log_on) {
							std::cout << ENTER << "\t" << "[json ~th] : " << j << " "
								<< "[jsontext] : " << jsontextUT->GetUserTypeList(j)->GetName().ToString() << ENTER;
						}

						if (mark2[j] == false && std::get<0>(temp = _Check(schema_eventUT, schemaUT->GetUserTypeList(utCount), jsontextUT->GetUserTypeList(j), wiz::load_data::LoadData::GetRealDir(jsontextUT->GetUserTypeList(j)->GetName().ToString(), jsontextUT->GetUserTypeList(j), &builder)))) {
							if (std::get<1>(temp).empty_ut == Option::EmptyUT_::ON && 0 == jsontextUT->GetUserTypeList(j)->GetIListSize()) {
								//
							}
							else if (Check(schema_eventUT, schemaUT->GetUserTypeList(utCount), jsontextUT->GetUserTypeList(j), depth + 1, log_on)) {
								//
							}
							else if (std::get<1>(temp).required == Option::Required_::OPTIONAL_) {
								jt_utCount--;
								validVisit[i] = true;
							}
							else {
								std::cout << "jsonText is not valid8" << ENTER;
								return false;
							}

							// visit vector? chk?
							validVisit[i] = true;
							varOptionVisit[i] = std::get<1>(temp);
							mark2[j] = true;

							pass = true;

							// check id, total id!
							if (std::get<1>(temp).id == Option::Id_::ID) {
								const std::string key_1 = jsontextUT->GetUserTypeList(j)->GetName().ToString();
								const std::string key_2 = "ut_name";
								const std::pair<std::string, std::string> key(key_1, key_2);

								if (check_id.find(key) == check_id.end()) {
									check_id.insert(key);
								}
								else {
									std::cout << "jsonText is not valid - ID9" << ENTER;
									return false;
								}
							}
							else if (std::get<1>(temp).id == Option::Id_::TOTAL_ID) {
								if (0 == depth) {
									std::cout << "ERROR schema is not valid5" << ENTER;
									return false;
								}

								const std::string key_1 = jsontextUT->GetUserTypeList(j)->GetName().ToString();
								const std::string key_2 = wiz::load_data::LoadData::GetRealDir(schemaUT->GetName().ToString(), schemaUT, &builder);
								const std::string key_3 = "ut_name";

								std::tuple<std::string, std::string, std::string> key(key_1, key_2, key_3);

								if (check_total_id.find(key) == check_total_id.end()) {
									check_total_id.insert(key);
								}
								else {
									std::cout << "jsonText is not valid _ ID10" << ENTER;
									return false;
								}
							}

							// check justone, (onemore)
							if (std::get<1>(temp).onemore == Option::OneMore_::JUSTONE) { // justone -> only for name! , not for value!
								if (check_justone.find(std::make_pair(jsontextUT->GetUserTypeList(j)->GetName().ToString(), std::string("ut"))) != check_justone.end()) {
									std::cout << "jsonText is not valid, justone is set, but not justone. 3" << ENTER;
									return false;
								}
								else {
									check_justone.insert(std::make_pair(jsontextUT->GetUserTypeList(j)->GetName().ToString(), std::string("ut")));
								}
								break;
							}
							else if (std::get<1>(temp).onemore == Option::OneMore_::ONEMORE) {
								jt_utCount++;
								use_onemore = true;
							}
							else {
								break;
							}
						}
					}
					if (false == pass) {
						Option var_option = OptionFrom(schemaUT->GetUserTypeList(utCount)->GetName().ToString());

						if (var_option.required == Option::Required_::OPTIONAL_) {
							jt_utCount--;

							validVisit[i] = true;
						}
						else {
							std::cout << "jsonText is not valid9 " << schemaUT->GetUserTypeList(utCount)->GetName().ToString() << ENTER;
							return false;
						}
					}

					if (use_onemore) {
						jt_utCount--;
					}
				}
				else if (order == Option::Order_::ON) {
					if (!chk_ct_ut)
					{
						break;
					}
					
					if (log_on) {
						std::cout << "[json ~th] : " << jt_utCount << " "
							<< "[jsontext] : " << jsontextUT->GetUserTypeList(jt_utCount)->GetName().ToString() << ENTER;
					}

					std::tuple<bool, Option> temp = _Check(schema_eventUT,
						schemaUT->GetUserTypeList(utCount), jsontextUT->GetUserTypeList(jt_utCount),
						wiz::load_data::LoadData::GetRealDir(jsontextUT->GetUserTypeList(jt_utCount)->GetName().ToString(), jsontextUT->GetUserTypeList(jt_utCount), &builder)
					);

					if (std::get<0>(temp)) {
						if (std::get<1>(temp).empty_ut == Option::EmptyUT_::ON && 0 == jsontextUT->GetUserTypeList(jt_utCount)->GetIListSize()) {
							//
						}
						else if (Check(schema_eventUT, schemaUT->GetUserTypeList(utCount), jsontextUT->GetUserTypeList(jt_utCount), depth + 1, log_on)) {
							validVisit[i] = true;

							// check id, total id!
							if (std::get<1>(temp).id == Option::Id_::ID) {
								const std::string key_1 = jsontextUT->GetUserTypeList(jt_utCount)->GetName().ToString();
								const std::string key_2 = "ut_name";
								const std::pair<std::string, std::string> key(key_1, key_2);

								if (check_id.find(key) == check_id.end()) {
									check_id.insert(key);
								}
								else {
									std::cout << "jsonText is not valid - ID11" << ENTER;
									return false;
								}
							}
							else if (std::get<1>(temp).id == Option::Id_::TOTAL_ID) {
								if (0 == depth) {
									std::cout << "ERROR schema is not valid6" << ENTER;
									return false;
								}

								const std::string key_1 = jsontextUT->GetUserTypeList(jt_utCount)->GetName().ToString();
								const std::string key_2 = wiz::load_data::LoadData::GetRealDir(schemaUT->GetName().ToString(), schemaUT, &builder);
								const std::string key_3 = "ut_name";

								std::tuple<std::string, std::string, std::string> key(key_1, key_2, key_3);

								if (check_total_id.find(key) == check_total_id.end()) {
									check_total_id.insert(key);
								}
								else {
									std::cout << "jsonText is not valid _ ID12" << ENTER;
									return false;
								}
							}

							// check justone, (onemore)
							if (std::get<1>(temp).onemore == Option::OneMore_::JUSTONE) { // justone -> only for name! , not for value!
								if (check_justone.find(std::make_pair(jsontextUT->GetUserTypeList(jt_utCount)->GetName().ToString(), std::string("ut"))) != check_justone.end()) {
									std::cout << "jsonText is not valid, justone is set, but not justone. 4" << ENTER;
									return false;
								}
								else {
									check_justone.insert(std::make_pair(jsontextUT->GetUserTypeList(jt_utCount)->GetName().ToString(), std::string("ut")));
								}
							}
							else if (std::get<1>(temp).onemore == Option::OneMore_::ONEMORE) {
								if (jt_utCount < jsontextUT->GetUserTypeListSize() - 1
									&& utCount < schemaUT->GetUserTypeListSize()
									&& jsontextUT->GetUserTypeList(jt_utCount)->GetName() == jsontextUT->GetUserTypeList(jt_utCount + 1)->GetName()
									) {
									if (1 != multiple_flag) {
										--i; utCount--;
									}
								}
							}
							else {
								//
							}
						}
						else if (std::get<1>(temp).required == Option::Required_::OPTIONAL_) {
							jt_utCount--;
							validVisit[i] = true;
						}
						else {
							std::cout << "jsonText is not valid11" << ENTER;
							return false;
						}
					}
					else if (std::get<1>(temp).required == Option::Required_::OPTIONAL_) {
						jt_utCount--;
						validVisit[i] = true;
					}
					else {
						std::cout << "jsonText is not valid12" << ENTER;
						return false;
					}

					if (1 == multiple_flag) {
						utCount--; i--;
					}
				}

				jt_utCount++;
				utCount++;
			}
		}

		if (multiple_flag && jsontextUT->GetUserTypeListSize() > 0 && jsontextUT->GetItemListSize() == 0) {
			utCount++;
		}
		else if (multiple_flag && jsontextUT->GetItemListSize() > 0 && jsontextUT->GetUserTypeListSize() == 0) {
			itCount++;
		}
		else if (multiple_flag) {
			return false;
		}

		if (jt_itCount != jsontextUT->GetItemListSize()) {
			std::cout << "jsonText is not valid13" << ENTER;
			return false;
		}
		if (jt_utCount != jsontextUT->GetUserTypeListSize()) {
			std::cout << "jsonText is not valid14 : " << jt_utCount << ENTER;
			return false;
		}

		if (itCount != schemaUT->GetItemListSize()) {
			std::cout << "jsonText is not valid15 : " << ENTER;
			return false;
		}
		if (utCount != schemaUT->GetUserTypeListSize()) {
			std::cout << "jsonText is not valid16 : " << ENTER;
			return false;
		}

		for (long long i = 0; i < validVisit.size(); ++i) {
			if (false == validVisit[i]) {
				std::cout << "jsonText is not valid17" << ENTER;
				return false;
			}
		}

		return true;
	}

	inline bool _Validate(const wiz::load_data::UserType& schemaFileUT)
	{
		// 1. $로 시작하는 것들을 따로 분리해서 별도의 UserType에 저장한다.
			// 1-1. $Event -> 따로 뽑아낸 다음 Event로 rename?
			// 1-2. $변수들 -> 따로 뽑아낸 다음 변수들로 rename?
		// 2. $로 시작하지는 않는 것들은 data check 내용이다?
			// 2-1. %로 시작하는 것을 찾아서 각각 수행한다. 
		wiz::load_data::UserType* jsontextUT;
		wiz::load_data::UserType schema_eventUT;
		wiz::load_data::UserType schemaUT;
		int itemCount = 0;
		int utCount = 0;

		for (int i = 0; i < schemaFileUT.GetIListSize(); ++i) {
			if (schemaFileUT.IsItemList(i)) { // itemtype
				const std::string name = wiz::ToString(schemaFileUT.GetItemList(itemCount).GetName());
				if (name.size() > 1 && name[0] == '$') {
					schema_eventUT.AddItem(name.substr(1), schemaFileUT.GetItemList(itemCount).Get(0));
				}
				else {
					schemaUT.AddItem(name, schemaFileUT.GetItemList(itemCount).Get(0));
				}
				itemCount++;
			}
			else { // usertype
				const std::string name = wiz::ToString(schemaFileUT.GetUserTypeList(utCount)->GetName());
				wiz::load_data::UserType temp = *schemaFileUT.GetUserTypeList(utCount);

				if (name.size() > 1 && name[0] == '$') {
					temp.SetName(wiz::ToString(temp.GetName()).substr(1));
					schema_eventUT.AddUserTypeItem(std::move(temp));
				}
				else {
					schemaUT.AddUserTypeItem(std::move(temp));
				}
				utCount++;
			}
		}

		// __init__ first init.
		std::string start_name;
		{
			wiz::ClauText jsonText;
			start_name = jsonText.excute_module("Main = { $call = { id = __init__ } }", &schema_eventUT, wiz::ExcuteData(), opt, 1); // 0 (remove events) -> 1 (revoke events?)

			if (start_name.empty()) {
				std::cout << "__init__ must return data`s start? folder?" << ENTER;
				exit(-1);
			}
		}

		jsontextUT = wiz::load_data::UserType::Find(&schema_eventUT, start_name, &builder).second[0];
		
		// for log?
		bool log_on = false;
		const bool chk = Check(&schema_eventUT, &schemaUT, jsontextUT, 0, log_on);

		//// debug
		//std::cout << schema_eventUT.ToString() << ENTER
		//	<< schemaUT.ToString() << ENTER;
		return chk;
	}

	inline bool Validate(const wiz::load_data::UserType& schema)
	{
		return _Validate(schema);
	}


	// to do
	//std::pair<bool, std::string> MakeSchema(const std::string& jsontext)
	//{
	//	// to do
	//}


	inline std::pair<bool, wiz::load_data::UserType> LoadFile(const std::string& fileName)
	{
		wiz::load_data::UserType ut;

		bool valid = wiz::load_data::LoadData::LoadDataFromFile3(fileName, ut, 0, 0);

		return { valid, std::move(ut) };
	}
	inline std::pair<bool, wiz::load_data::UserType> LoadJsonFile(const std::string& fileName)
	{
		wiz::load_data::UserType ut;

		bool valid = wiz::load_data::LoadData::LoadDataFromFileFastJson(fileName, ut, 0, 0);

		return { valid, std::move(ut) };
	}

	// todo SaveJsonFile? for Make Schema?
	inline bool SaveFile(const std::string& fileName, wiz::load_data::UserType* data)
	{
		wiz::load_data::LoadData::SaveWizDB(*data, fileName, "1");
		return true;
	}

}

int main(int argc, char* argv[])
{
	std::string option;
	wiz::load_data::UserType schema; // argv[3]?
	wiz::load_data::UserType jsontext; // argv[2]
	std::string fileName; // to save

	// -v : version? - to do
	// -V : Validate.
	// -M : Make schema. // removal?
	if (argc == 3 && 0 == strcmp("-V", argv[1])) {
		option = "-V";

		//auto chk_jsontext = Lint::LoadFile(argv[2]);
		auto chk_schema = Lint::LoadFile(argv[2]); // 3 -> 2

		if (chk_schema.first) {
			schema = std::move(chk_schema.second);
		}
		else {
			std::cout << "schema load fail" << ENTER;
			return 1;
		}
		//if (chk_jsontext.first) {
		//	jsontext = std::move(chk_jsontext.second);
		//}
		//else {
		//	std::cout << "jsontext load fail" << ENTER;
		//	return 2;
		//}
	}
	else if (argc == 4 && 0 == strcmp("-M", argv[1])) {
		option = "-M";

		auto chk_jsontext = Lint::LoadJsonFile(argv[2]);

		if (chk_jsontext.first) {
			jsontext = std::move(chk_jsontext.second);
		}
		else {
			std::cout << "jsontext load fail" << ENTER;
			return 3;
		}
		fileName = argv[3];
	}

	if (option == "-V") {
		auto chk = Lint::Validate(schema);
		if (chk) {
			std::cout << ENTER <<  "success" << ENTER;
		}
		else {
			std::cout << ENTER << "fail" << ENTER;
		}
	}
	else if (option == "-M") {
		//	auto chk = MakeSchema(jsontext);
		//	if (chk.first) {
		//		schema = chk.second;

		//		if (SaveFile(fileName, schema)) {

		//		}
		//		else {
		//			std::cout << "save fail" << ENTER;
		//			return 4;
		//		}

		//		std::cout << "success" << ENTER;
		//	}
		//	else {
		//		std::cout << "fail" << ENTER;
		//	}
	}
	else {
		std::cout << "it is not valid option" << ENTER;
	}

	return 0;
}


