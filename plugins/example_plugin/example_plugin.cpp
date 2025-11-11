#include "../../include/plugins/plugin_manager.h"
#include <iostream>

using namespace customos::plugins;

class ExamplePlugin : public IPlugin {
public:
    std::string get_name() const override {
        return "ExamplePlugin";
    }

    std::string get_version() const override {
        return "1.0.0";
    }

    std::string get_description() const override {
        return "An example plugin demonstrating the plugin API";
    }

    std::string get_author() const override {
        return "CustomOS Team";
    }

    bool initialize() override {
        std::cout << "ExamplePlugin initialized!\n";
        // Register commands here
        return true;
    }

    void shutdown() override {
        std::cout << "ExamplePlugin shutdown!\n";
    }

    std::vector<std::string> get_commands() const override {
        return {"example-hello", "example-info"};
    }
};

// Export the plugin
EXPORT_PLUGIN(ExamplePlugin)
