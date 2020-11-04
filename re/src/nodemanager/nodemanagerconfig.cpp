#include "nodemanagerconfig.h"
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <optional>
namespace re::node_manager {

auto NodeConfig::FromIstream(std::basic_istream<char>& file_contents)
    -> std::optional<NodeConfig>
{
    namespace po = boost::program_options;
    po::options_description config_file_options("Config file options");
    std::string control_ip_address_str{};
    std::string data_ip_address_str{};
    std::string env_manager_registration_endpoint_string{};
    std::string library_root_str{};
    std::string node_uuid_str{};
    std::string re_bin_path{};
    std::string hostname_string{};

    config_file_options.add_options()("control_ip_address,i",
                                      po::value<std::string>(&control_ip_address_str)->required(),
                                      "node ip address");

    config_file_options.add_options()("data_ip_address,i",
                                      po::value<std::string>(&data_ip_address_str)->required(),
                                      "node ip address");
    config_file_options.add_options()(
        "environment_manager_registration_endpoint,e",
        po::value<std::string>(&env_manager_registration_endpoint_string)->required(),
        "environment manager registration endpoint eg 192.168.111.1:5672");
    config_file_options.add_options()("library_root,l",
                                      po::value<std::string>(&library_root_str)->required(),
                                      "Model library root directory");

    config_file_options.add_options()("uuid,u", po::value<std::string>(&node_uuid_str),
                                      "Node uuid");
    config_file_options.add_options()("host_name,h", po::value<std::string>(&hostname_string));
    config_file_options.add_options()("re_bin_path,p",
                                      po::value<std::string>(&re_bin_path)->required());

    try {
        po::variables_map parsed_options;
        po::store(po::parse_config_file(file_contents, config_file_options), parsed_options);
        po::notify(parsed_options);

        // If we don't have a uuid, a random one is initialised
        sem::types::Uuid uuid{};
        if(parsed_options.count("uuid")) {
            // Read our uuid from our config file
            uuid = sem::types::Uuid{node_uuid_str};
        }

        std::optional<std::string> hostname;
        if(hostname_string.empty()) {
            hostname = std::nullopt;
        } else {
            hostname = hostname_string;
        }

        return {{sem::types::Ipv4(control_ip_address_str), sem::types::Ipv4(data_ip_address_str),
                 sem::types::SocketAddress(env_manager_registration_endpoint_string),
                 library_root_str, uuid, hostname, re_bin_path}};
    } catch(const std::exception& ex) {
        std::cout << config_file_options << std::endl;
        std::string error_message{"Error parsing config file: "};
        error_message += ex.what();
        throw std::invalid_argument(error_message);
    }
}

auto NodeConfig::SaveConfigFile(const NodeConfig& config) -> void
{
    std::ofstream file{config.file_path};
    if(file.is_open()) {
        file << config;
        file.close();
    } else {
        std::cerr << "Could not save node manager config file: " << std::endl;
        std::cerr << config;
        throw std::runtime_error("Could not save nodemanager config file!");
    }
}
auto NodeConfig::ParseArguments(int argc, char** argv)
    -> std::optional<std::string>
{
    namespace po = boost::program_options;
    po::options_description command_line_options("Node manager options");
    std::string config_file_path{};
    command_line_options.add_options()(
        "config_file,f", boost::program_options::value<std::string>(&config_file_path)->required(),
        "Config file path");
    command_line_options.add_options()("help,h", "help");

    po::variables_map parsed_options;
    po::store(po::parse_command_line(argc, argv, command_line_options), parsed_options);
    po::notify(parsed_options);

    // If we encounter "help" in our args, print possible args and return nullopt.
    // This isn't a failure, so we don't throw. Onus is on caller to check that they got a
    //  populated optional
    if(parsed_options.count("help")) {
        std::cout << command_line_options << std::endl;
        return std::nullopt;
    }
    return config_file_path;
}
auto NodeConfig::HandleArguments(int argc, char** argv)
    -> std::optional<NodeConfig>
{
    auto config_file_path = ParseArguments(argc, argv);
    if(config_file_path) {
        std::ifstream config_file_stream(config_file_path.value());
        auto config = FromIstream(config_file_stream);
        config->file_path = config_file_path.value();
        return config;
    }
    return std::nullopt;
}
}