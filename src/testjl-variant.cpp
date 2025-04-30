#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <iostream>
#include <jsonlogic/logic.hpp>
#include <string>

int main() {
  // from tests
  std::string expr_str_f = R"({"==":[1,2]})";
  std::string expr_str_t = R"({"==":[1,1]})";
  std::string expr_str_v = R"({"<":[{"var":"x"},10]})";
  std::string expr_str_v2 = R"({"<":[{"var":["x"]},{"var":"y"}]})";

  auto jv_f = boost::json::parse(expr_str_f);
  auto jv_t = boost::json::parse(expr_str_t);
  auto jv_v = boost::json::parse(expr_str_v);

  auto jv_v2 = boost::json::parse(expr_str_v2);

  auto data_obj_t = boost::json::object();
  data_obj_t["x"] = 5;
  auto data_t = boost::json::value_from(data_obj_t);

  auto data_obj_f = boost::json::object();
  data_obj_f["x"] = 15;
  auto data_f = boost::json::value_from(data_obj_f);

  auto data_obj_f2 = boost::json::object();
  data_obj_f2["x"] = 20;
  data_obj_f2["y"] = 15;
  auto data_f2 = boost::json::value_from(data_obj_f2);

  auto data_obj_t2 = boost::json::object();
  data_obj_t2["x"] = 5;
  data_obj_t2["y"] = 15;
  auto data_t2 = boost::json::value_from(data_obj_t2);

  std::cout << "data_t: " << data_t << std::endl;
  std::cout << "data_f: " << data_f << std::endl;
  auto v_f = jsonlogic::apply(jv_f, boost::json::value{});
  auto v_t = jsonlogic::apply(jv_t, boost::json::value{});
  auto v_v_t = jsonlogic::apply(jv_v, data_t);
  auto v_v_f = jsonlogic::apply(jv_v, data_f);
  std::cout << "jv_f: " << v_f << std::endl;
  std::cout << "jv_t: " << v_t << std::endl;
  std::cout << "jv_v_t: " << v_v_t << std::endl;
  std::cout << "jv_v_f: " << v_v_f << std::endl;

  auto v_v2_t = jsonlogic::apply(jv_v2, data_t2);
  auto v_v2_f = jsonlogic::apply(jv_v2, data_f2);
  std::cout << "jv_v2_t: " << v_v2_t << std::endl;
  std::cout << "jv_v2_f: " << v_v2_f << std::endl;

  std::string str = "Hello, World!";
  std::cout << str << std::endl;
  return 0;
}