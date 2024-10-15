//
// Created by firewolf304 on 29.01.24.
//

#ifndef WIFIPARSER_PARSING_LIBNM_HPP
#define WIFIPARSER_PARSING_LIBNM_HPP
#include "../include.hpp"
namespace firewolf::wifi {
#define NM_CLIENT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), NM_TYPE_CLIENT, NMClientPrivate))
    struct APConf;
    class parsing_libnm{
        class utils {
        public:
            static std::string gbyteArray_to_string( GByteArray * array );
            static std::string gbytes_to_string( GBytes * array );
            static std::string gbytes_to_string( std::shared_ptr<GBytes *> array ) ;
            std::shared_ptr<NMAccessPoint *> nm_access_point_convert_to_smart_point(NMAccessPoint * point);
            std::shared_ptr<GBytes *> gbytes_get_ssid_smart_point(std::shared_ptr<NMAccessPoint *> point);
        };
    public:
        utils tools;
        std::shared_ptr<NMDevice*> device;
        NMClient *client;
        GError *error = nullptr;
        parsing_libnm() ;
        ~parsing_libnm();
        std::vector<gpointer> get_devices();
        std::vector<std::string> get_name_devices();
        std::unordered_map<std::string,std::unordered_map<std::string, std::string>> get_data_devices();
        std::vector<NMDevice *> get_devices_fixed();
        std::vector<gpointer> get_devices(NMClient * client_ptr) ;
        std::vector<NMDevice *> get_devices_fixed(NMClient * client_ptr);


        std::shared_ptr<NMDevice * > get_device_by_name_smart(std::string name ) const {
            return std::make_shared<NMDevice *>( nm_client_get_device_by_iface(this->client, name.c_str()) );
        }

        void output_all_device_states (NMDevice * dev);
        std::unordered_map<std::string, std::string> get_device_info(NMDevice * dev);

        NMDevice * get_device_by_name(std::string & name ) const {
            return ( nm_client_get_device_by_iface(this->client, name.c_str()) );
        }
        std::shared_ptr<NMDeviceWifi * > get_device_wifi_by_name_smart(std::string name ) const {
            return std::make_shared<NMDeviceWifi *>( NM_DEVICE_WIFI(nm_client_get_device_by_iface(this->client, name.c_str())) );
        }
        NMDeviceWifi * get_device_wifi_by_name(std::string  name ) const {
            return NM_DEVICE_WIFI(nm_client_get_device_by_iface(this->client, name.c_str()));
        }
        std::vector<gpointer> get_access_points( std::shared_ptr<NMDevice*> device ) {
            auto endPoints = nm_device_wifi_get_access_points(NM_DEVICE_WIFI(*device) );
            auto AccessPoints = std::vector<gpointer>(endPoints->pdata, endPoints->pdata + endPoints->len);
            return AccessPoints;
        }
        std::vector<gpointer> get_access_points() {
            this->device = get_best_device_smart();
            if(this->device != nullptr) {
                return get_access_points(device);
            }
            throw std::runtime_error("No wifi device");
        }
        std::shared_ptr<NMDevice * > get_best_device_smart() {
            if(!this->client) {
                throw std::runtime_error("Client closed");
            }
            auto devices = get_devices();
            for(auto device : devices) {
                auto dev = static_cast<NMDevice *>(device);
                if(dev && NM_IS_DEVICE_WIFI(dev)) {
                    return std::make_shared<NMDevice *>(dev);
                }
            }
            return nullptr;
        }
        std::shared_ptr<NMDeviceWifi *> get_best_device_wifi_smart() {
            std::shared_ptr<NMDevice *> dev = get_best_device_smart();
            return std::make_shared<NMDeviceWifi *>( NM_DEVICE_WIFI(*dev) );
        }
        NMDeviceWifi * get_best_device_wifi() {
            std::shared_ptr<NMDevice *> dev = get_best_device_smart();
            return NM_DEVICE_WIFI(*dev) ;
        }
        std::vector<NMAccessPoint *> get_access_points_fixed( std::shared_ptr<NMDevice*> device ) {
            auto endPoints = nm_device_wifi_get_access_points(NM_DEVICE_WIFI(*device) );
            auto AccessPoints = std::vector<gpointer>(endPoints->pdata, endPoints->pdata + endPoints->len);
            std::vector<NMAccessPoint *> ret;
            for(auto ap : AccessPoints) {
                ret.push_back(static_cast<NMAccessPoint * >(ap));
            }
            return ret;
        }
        std::vector<NMAccessPoint *> get_access_points_fixed() {
            std::shared_ptr<NMDevice*> device = get_best_device_smart();
            if(device != nullptr) {
                return get_access_points_fixed(device);
            }
            throw std::runtime_error("No wifi device");
        }
        enum full_info_output : int{
            // ssid string
            SSID = 0,
            // mac string
            BSSID = 1,
            // flasg string
            FLAGS = 2,
            // freq guint32
            FREQ = 3,
            // mode string
            MODE = 4,
            // rsn string
            RSN_FLAGS = 5,
            // security string
            SEC_TYPE = 6,
            // wpa string
            WPA_TYPE = 7,
            // signal strength guint32
            POWER = 8,
            // delimiter '|'
            DEL = 9,
            // equal sign '='
            EQUAL = 10,
            // bracket '[' or ']'
            BRACKET = 11,
            // space ' '
            SPACE = 12,
            // hyphen '-'
            DEF = 13,
            // max power
            MAX_POWER = 14,
            // slash '/'
            SLASH = 15,
            // max bitrate guint32
            MAX_BITRATE = 16,
        };

        std::string get_device_type(NMDeviceType type);
        std::string get_device_capabilities(NMDeviceCapabilities capabilities);

        std::string get_access_point_full_info(std::shared_ptr< NMAccessPoint *> point, std::vector<full_info_output> format = {}) ;
        std::vector<std::string> get_access_point_full_info_vector(std::shared_ptr< NMAccessPoint *> point, std::vector<full_info_output> format = {});

        enum setting_type {
            setting_none = -1,
            setting_wireless = 0,
            setting_wireless_secutiry = 1,
            setting_802_11x = 2
        };
        enum value_type {
            value_none = -1,
            value_int = 0,
            value_gbyte = 1,
            value_string = 2
        };

        /*
         Map parameters:
             name, <string_name, gbyte_value, value, parameter_value, setting_type, value_type>
         Values:
             SSID = string, GBytes
             BSSID = string
             FREQ = guint32
             POWER = guint32
             MAX_BITRATE = guint32
             FLAGS = string, int
             MODE = string, int
             RSN = string, int
             SEC_TYPE = string, int
             WPA_TYPE = string
             https://developer-old.gnome.org/libnm/stable/NMSettingWirelessSecurity.html
         */
        typedef std::map<std::string, std::tuple<std::string, GBytes *, int, const char *, setting_type, value_type>> large_value;
        [[maybe_unused]] large_value get_access_point_full_info_map(std::shared_ptr< NMAccessPoint *> point) ;
        std::string get_access_point_full_info(NMAccessPoint * point, std::vector<full_info_output> format = {}) {
            return get_access_point_full_info( std::shared_ptr<NMAccessPoint *>(&point), format );
        }
        struct _dependencies {
            bool s_wireless = false,
                    s_8021x = false,
                    s_sec = false;
        };
        struct APConf {
            _dependencies dependencies;
            std::map<std::string, std::tuple<std::string, GBytes *, int, const char *, parsing_libnm::setting_type, parsing_libnm::value_type>> info;
            //get all data using default pattern
            void get_all(parsing_libnm & libnm, std::shared_ptr<_NMAccessPoint *> point);

        };
        std::vector<std::pair<NMAccessPoint *, large_value>> get_access_points_large() {
            std::vector<std::pair<NMAccessPoint *, large_value>> ret;
            auto aps = get_access_points_fixed();
            for(auto d : aps ) {
                ret.push_back({d, get_access_point_full_info_map(tools.nm_access_point_convert_to_smart_point(d)) });
            }
            return ret;
        }
        void simple_connection(std::string iface, std::string password, std::shared_ptr<NMAccessPoint *> point) ;
        void simple_connection(std::string password, std::shared_ptr<NMAccessPoint *> point) {
            std::shared_ptr<NMDeviceWifi *> dev = this->get_best_device_wifi_smart();
            this->simple_connection(dev, password, point);
        }
        void simple_connection(std::shared_ptr<NMDeviceWifi *> dev, std::string password, std::shared_ptr<NMAccessPoint *> point);

        struct specific_data {
            std::shared_ptr<GMainLoop *> loop;
            std::shared_ptr<NMDeviceWifi *> dev;
            std::shared_ptr<NMAccessPoint *> point;
            std::shared_ptr<NMConnection *> conn;
            std::string password;
            std::shared_ptr<bool> verified;
        };
        void spam_connestion() ;
    private:
        static void done (GDBusProxy *proxy, gchar      *sender_name, gchar      *signal_name,
                          GVariant   *parameters,
                          gpointer    user_data) {

            //auto client = reinterpret_cast<NMClient *>(source_object);

        }

        static void check_auth (GObject *source_object, GAsyncResult *res, gpointer user_data) {
            GError *error = nullptr;
            GMainLoop * main_loop = reinterpret_cast<GMainLoop *>(user_data);
            if (!nm_client_add_and_activate_connection_finish(reinterpret_cast<NMClient *>(source_object), res,&error)) {
                std::cerr << "Connection failed:" << error->message << std::endl;
            } else {
                std::cerr << "Connection success:" << std::endl;
                if(nm_client_connectivity_check_get_enabled(reinterpret_cast<NMClient *>(source_object))) {
                    std::cerr << "connecting" << std::endl;

                    GMainLoop * loop = g_main_loop_new(nullptr, false);
                    //nm_client_check_connectivity_async(reinterpret_cast<NMClient *>(source_object), nullptr, done, loop);
                    g_main_loop_run(loop);
                    g_object_unref(loop);
                }
            }
            g_error_free(error);
            g_clear_error(&error);
            g_main_loop_quit(main_loop);

        }
        static void check_auth2 (GObject *source_object, GAsyncResult *res, gpointer user_data) {
            GError    *     error = nullptr;
            auto            client = reinterpret_cast<NMClient *>(source_object);
            auto            user = reinterpret_cast<specific_data*>(user_data);
            GMainLoop *     main_loop = reinterpret_cast<GMainLoop *>(*user->loop);
            GVariant  *     out_result;
            NMActiveConnection * ac = nm_client_get_activating_connection(client);
            GVariantIter    iter;
            gchar     *     key;
            GVariant  *     value;
            /*while(state == NM_DEVICE_STATE_CONFIG || state ==NM_DEVICE_STATE_PREPARE) {
                state = nm_device_get_state(NM_DEVICE(*user->dev));
            }*/
            if (nm_client_add_and_activate_connection2_finish(client, res, &out_result ,&error) == nullptr) {
                try {
                    if(error) {
                        std::cerr << "Connection failed: " << error->message << std::endl;
                    }
                    std::cerr << "Connection failed (NULL error)" << std::endl;
                } catch (std::runtime_error & e) {
                    std::cerr << "(NULL error detector)" << std::endl;
                }
            } else {
                std::cerr << "Connection success" << std::endl;
                bool stop = false;
                GMainContext * dbus = nm_client_get_main_context(client); //nm_client_get_dbus_connection(client);
                std::cerr << "connecting" << std::endl;
                /*NMActiveConnectionState state;
                while( (state = nm_active_connection_get_state(ac)) != NM_ACTIVE_CONNECTION_STATE_ACTIVATING) {
                    if(state == NM_ACTIVE_CONNECTION_STATE_ACTIVATED) {
                        stop = true;
                    }
                    else {
                        break;
                    }
                }*/

                if(stop) {
                    std::cerr << "connected" << std::endl;
                    *user->verified = true;
                }
                std::cout << "Exit\n";
                //g_object_unref(loop);

            }
            // get out_result

            g_variant_iter_init(&iter, out_result);
            while (g_variant_iter_next(&iter, "{sv}", &key, &value)) {
                std::cout << "Key: " << key << std::endl;
                const GVariantType *value_type = g_variant_get_type(value);
                if (g_variant_type_equal(value_type, G_VARIANT_TYPE_INT32)) {
                    gint32 int_value;
                    g_variant_get(value, "i", &int_value);
                    std::cout << "Value (string): " <<int_value << std::endl;
                } else if (g_variant_type_equal(value_type, G_VARIANT_TYPE_STRING)) {
                    //const gchar *string_value;
                    std::string string_value;
                    g_variant_get(value, "s", &string_value);
                    std::cout << "Value (string): " <<string_value << std::endl;
                }
                g_free(key);
            }


            // exit
            g_clear_error(&error);
            g_main_loop_quit(main_loop);

        }
        static void error_dumper(int code) { throw std::runtime_error("Something wrong, status code: " + std::to_string(code)); }
    };

}
#endif //WIFIPARSER_PARSING_LIBNM_HPP
