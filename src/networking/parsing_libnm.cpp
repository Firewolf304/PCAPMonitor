//
// Created by firewolf on 06.07.24.
//
#include "../../include/networking/parsing_libnm.hpp"

std::string firewolf::wifi::parsing_libnm::utils::gbyteArray_to_string(GByteArray *array) {
    std::string text(array->data, array->data + array->len);
    return text;
}

std::string firewolf::wifi::parsing_libnm::utils::gbytes_to_string(GBytes *array) {
    if(array) {
        //std::cout << g_bytes_get_size(array) << std::endl;
        g_bytes_ref(array);
        auto result = g_bytes_unref_to_array(array);
        if(result) {
            std::string text(result->data, result->data + result->len);
            g_byte_array_free(result, false);
            return text;
        }
    }
    return "";
}

std::string firewolf::wifi::parsing_libnm::utils::gbytes_to_string(std::shared_ptr<GBytes *> array) {
    if( *array) {
        g_bytes_ref(*array);
        //std::cout << g_bytes_get_size(*array) << std::endl;
        auto result = g_bytes_unref_to_array(*array);
        if(result) {
            std::string text(result->data, result->data + result->len);
            g_byte_array_free(result, false);
            return text;
        }
    }
    return "";
}

std::shared_ptr<NMAccessPoint *> firewolf::wifi::parsing_libnm::utils::nm_access_point_convert_to_smart_point(NMAccessPoint *point) {
    return std::move(std::make_shared<NMAccessPoint *>( point));
}

std::shared_ptr<GBytes *>
firewolf::wifi::parsing_libnm::utils::gbytes_get_ssid_smart_point(std::shared_ptr<NMAccessPoint *> point) {
    return std::move(std::make_shared<GBytes *>( nm_access_point_get_ssid(*point)));
}

firewolf::wifi::parsing_libnm::parsing_libnm() {
    signal(SIGSEGV, error_dumper);
    client = nm_client_new(nullptr, &error);
    if (!client) {

        g_error_free(error);
        throw std::runtime_error("Error: Could not connect to NetworkManager: " + (std::string)error->message);
    }
    sleep(1);
}

firewolf::wifi::parsing_libnm::~parsing_libnm() {
    if(this->client)
        g_object_unref(this->client);
    g_clear_error(&this->error);
    glib_autoptr_clear_GError(this->error);
}

std::vector<gpointer> firewolf::wifi::parsing_libnm::get_devices() {
    if (!this->client) {
        throw std::runtime_error("Client closed");
    }
    std::unique_ptr<const GPtrArray*> devices_ptr = std::make_unique<const GPtrArray*>(nm_client_get_devices(client));
    auto ret = std::vector<gpointer>((*devices_ptr)->pdata, (*devices_ptr)->pdata + (int)(*devices_ptr)->len);
    return ret;
}

std::vector<NMDevice *> firewolf::wifi::parsing_libnm::get_devices_fixed() {
    auto row = get_devices();
    std::vector<NMDevice * > mass;
    for(auto data : row) {
        mass.push_back( static_cast<NMDevice *>(data) );
    }
    return mass;
}

std::vector<gpointer> firewolf::wifi::parsing_libnm::get_devices(NMClient * client_ptr)  {
    if (!client_ptr) {
        throw std::runtime_error("Client closed");
    }
    std::unique_ptr<const GPtrArray*> devices_ptr = std::make_unique<const GPtrArray*>(nm_client_get_devices(client_ptr));
    auto ret = std::vector<gpointer>((*devices_ptr)->pdata, (*devices_ptr)->pdata + (int)(*devices_ptr)->len);
    return ret;
}

std::vector<NMDevice *> firewolf::wifi::parsing_libnm::get_devices_fixed(NMClient * client_ptr) {
    auto row = get_devices(client_ptr);
    std::vector<NMDevice * > mass;
    for(auto data : row) {
        mass.push_back( static_cast<NMDevice *>(data) );
    }
    return mass;
}

std::string firewolf::wifi::parsing_libnm::get_access_point_full_info(std::shared_ptr<NMAccessPoint *> point,
                                                                      std::vector<full_info_output> format) {
    std::string ssid = tools.gbytes_to_string( tools.gbytes_get_ssid_smart_point( point) );
    std::string bssid = (std::string)nm_access_point_get_bssid(*point);
    NM80211ApFlags flags_enum = nm_access_point_get_flags(*point);
    std::string flags = "";
    if(flags_enum & NM_802_11_AP_FLAGS_NONE) {
        flags = "NONE";
    }
    else if(flags_enum & NM_802_11_AP_FLAGS_PRIVACY) {
        flags = "PRIVACY";
    }
    else if(flags_enum & NM_802_11_AP_FLAGS_WPS) {
        flags = "WPS";
    }
    else if(flags_enum & NM_802_11_AP_FLAGS_WPS_PBC) {
        flags = "WPS_PBC";
    }
    else if(flags_enum & NM_802_11_AP_FLAGS_WPS_PIN) {
        flags = "WPS_PIN";
    }

    guint32 freq = nm_access_point_get_frequency(*point);
    NM80211Mode mode_enum =  nm_access_point_get_mode(*point);
    std::string mode = "";
    if (mode_enum == NM_802_11_MODE_ADHOC) {
        mode = "ADHOC";
    }
    else if (mode_enum & NM_802_11_MODE_AP) {
        mode = "AP";
    }
    else if (mode_enum & NM_802_11_MODE_INFRA) {
        mode = "INFRA";
    }
    else if (mode_enum & NM_802_11_MODE_MESH) {
        mode = "MESH";
    }
    else if (mode_enum & NM_802_11_MODE_UNKNOWN) {
        mode = "UNKNOWN";
    }
    NM80211ApSecurityFlags rsn_flags_enum = nm_access_point_get_rsn_flags(*point);
    std::string rsn_flags = "";
    if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_PSK ) {
        rsn_flags = "PSK";
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_802_1X ) {
        rsn_flags = "802_1X";
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_EAP_SUITE_B_192 ) {
        rsn_flags = "EAP_SUITE_B_192";
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_OWE ) {
        rsn_flags = "OWE";
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_OWE_TM ) {
        rsn_flags = "OWE_TM";
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_SAE ) {
        rsn_flags = "SAE";
    }
    else {
        rsn_flags = "UNKNOW";
    }

    guint32 power = nm_access_point_get_strength(*point);
    guint32 max_bitrate = nm_access_point_get_max_bitrate(*point);
    NM80211ApSecurityFlags sec_type_enum = nm_access_point_get_wpa_flags(*point);
    std::string sec_type = "";
    if (sec_type_enum & NM_802_11_AP_SEC_PAIR_WEP40) {
        sec_type = "WEP40";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_PAIR_WEP104) {
        sec_type = "WEP104";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_PAIR_TKIP) {
        sec_type = "TKIP";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_PAIR_CCMP) {
        sec_type = "CCMP";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_GROUP_WEP40) {
        sec_type = "WEP40";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_GROUP_WEP104) {
        sec_type = "WEP104";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_GROUP_TKIP) {
        sec_type = "TKIP";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_GROUP_CCMP) {
        sec_type = "CCMP";
    }
    else {
        sec_type = "UNKNOW";
    }

    std::string WPA_type = "";
    if (sec_type_enum & NM_802_11_AP_SEC_PAIR_WEP40 || sec_type_enum & NM_802_11_AP_SEC_PAIR_WEP104 ||
        rsn_flags_enum & NM_802_11_AP_SEC_PAIR_WEP40 || rsn_flags_enum & NM_802_11_AP_SEC_PAIR_WEP104) {
        WPA_type = "WEP";
    } else if (sec_type_enum & NM_802_11_AP_SEC_PAIR_TKIP || sec_type_enum & NM_802_11_AP_SEC_PAIR_CCMP ||
               rsn_flags_enum & NM_802_11_AP_SEC_PAIR_TKIP || rsn_flags_enum & NM_802_11_AP_SEC_PAIR_CCMP) {
        WPA_type = "WPA/WPA2";
    } else {
        WPA_type = "UNKNOW";
    }
    if(format.empty()) {
        auto ret = ssid + " (" + bssid + ") => [" + sec_type + " = " + WPA_type + "-" + rsn_flags + "]";
        return ret;
    }
    else {
        std::string ret = "";
        int bracket = 0;
        for(auto type : format) {
            switch(type) {
                case SSID: {
                    if(ssid.empty()) {
                        ret += "[null]";
                    }
                    else{
                        ret += ssid;
                    }
                } break;
                case BSSID:
                    ret += bssid; break;
                case FLAGS:
                    ret += flags; break;
                case FREQ:
                    ret += std::to_string(freq); break;
                case MODE:
                    ret += mode; break;
                case RSN_FLAGS:
                    ret += rsn_flags; break;
                case SEC_TYPE:
                    ret += sec_type; break;
                case WPA_TYPE:
                    ret += WPA_type; break;
                case POWER:
                    ret += std::to_string(power); break;
                case DEL:
                    ret += "|"; break;
                case EQUAL:
                    ret += "=";break;
                case BRACKET:
                {
                    if(bracket <= 0) {
                        ret += "[";
                        bracket++;
                    }
                    else {
                        ret+= "]";
                        bracket--;
                    }
                } break;
                case SPACE:
                    ret += " "; break;
                case DEF:
                    ret += "-"; break;
                case SLASH:
                    ret += "/"; break;
                case MAX_POWER:
                    ret += "100"; break;
                case MAX_BITRATE:
                    ret += std::to_string(max_bitrate); break;
            }
        }
        return ret;
    }

}

std::vector<std::string>
firewolf::wifi::parsing_libnm::get_access_point_full_info_vector(std::shared_ptr<NMAccessPoint *> point,
                                                                 std::vector<full_info_output> format) {
    std::string ssid = tools.gbytes_to_string( tools.gbytes_get_ssid_smart_point( point) );
    std::string bssid = (std::string)nm_access_point_get_bssid(*point);
    NM80211ApFlags flags_enum = nm_access_point_get_flags(*point);
    std::string flags = "";
    if(flags_enum & NM_802_11_AP_FLAGS_NONE) {
        flags = "NONE";
    }
    else if(flags_enum & NM_802_11_AP_FLAGS_PRIVACY) {
        flags = "PRIVACY";
    }
    else if(flags_enum & NM_802_11_AP_FLAGS_WPS) {
        flags = "WPS";
    }
    else if(flags_enum & NM_802_11_AP_FLAGS_WPS_PBC) {
        flags = "WPS_PBC";
    }
    else if(flags_enum & NM_802_11_AP_FLAGS_WPS_PIN) {
        flags = "WPS_PIN";
    }

    guint32 freq = nm_access_point_get_frequency(*point);
    NM80211Mode mode_enum =  nm_access_point_get_mode(*point);
    std::string mode = "";
    if (mode_enum == NM_802_11_MODE_ADHOC) {
        mode = "ADHOC";
    }
    else if (mode_enum & NM_802_11_MODE_AP) {
        mode = "AP";
    }
    else if (mode_enum & NM_802_11_MODE_INFRA) {
        mode = "INFRA";
    }
    else if (mode_enum & NM_802_11_MODE_MESH) {
        mode = "MESH";
    }
    else if (mode_enum & NM_802_11_MODE_UNKNOWN) {
        mode = "UNKNOWN";
    }
    NM80211ApSecurityFlags rsn_flags_enum = nm_access_point_get_rsn_flags(*point);
    std::string rsn_flags = "";
    if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_PSK ) {
        rsn_flags = "PSK";
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_802_1X ) {
        rsn_flags = "802_1X";
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_EAP_SUITE_B_192 ) {
        rsn_flags = "EAP_SUITE_B_192";
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_OWE ) {
        rsn_flags = "OWE";
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_OWE_TM ) {
        rsn_flags = "OWE_TM";
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_SAE ) {
        rsn_flags = "SAE";
    }
    else {
        rsn_flags = "UNKNOW";
    }

    guint32 power = nm_access_point_get_strength(*point);
    guint32 max_bitrate = nm_access_point_get_max_bitrate(*point);
    NM80211ApSecurityFlags sec_type_enum = nm_access_point_get_wpa_flags(*point);
    std::string sec_type = "";
    if (sec_type_enum & NM_802_11_AP_SEC_PAIR_WEP40) {
        sec_type = "WEP40";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_PAIR_WEP104) {
        sec_type = "WEP104";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_PAIR_TKIP) {
        sec_type = "TKIP";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_PAIR_CCMP) {
        sec_type = "CCMP";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_GROUP_WEP40) {
        sec_type = "WEP40";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_GROUP_WEP104) {
        sec_type = "WEP104";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_GROUP_TKIP) {
        sec_type = "TKIP";
    }

    else if (sec_type_enum & NM_802_11_AP_SEC_GROUP_CCMP) {
        sec_type = "CCMP";
    }
    else {
        sec_type = "UNKNOW";
    }

    std::string WPA_type = "";
    if (sec_type_enum & NM_802_11_AP_SEC_PAIR_WEP40 || sec_type_enum & NM_802_11_AP_SEC_PAIR_WEP104 ||
        rsn_flags_enum & NM_802_11_AP_SEC_PAIR_WEP40 || rsn_flags_enum & NM_802_11_AP_SEC_PAIR_WEP104) {
        WPA_type = "WEP";
    } else if (sec_type_enum & NM_802_11_AP_SEC_PAIR_TKIP || sec_type_enum & NM_802_11_AP_SEC_PAIR_CCMP ||
               rsn_flags_enum & NM_802_11_AP_SEC_PAIR_TKIP || rsn_flags_enum & NM_802_11_AP_SEC_PAIR_CCMP) {
        WPA_type = "WPA/WPA2";
    } else {
        WPA_type = "UNKNOW";
    }
    if(format.empty()) {
        std::vector<std::string> ret = {ssid, "(", bssid, ")", "=>", "[", sec_type, "=", WPA_type, "-", rsn_flags, "]" };
        return ret;
    }
    else {
        std::vector<std::string> ret;
        int bracket = 0;
        for(auto type : format) {
            switch(type) {
                case SSID: {
                    if(ssid.empty()) {
                        ret.push_back("[null]");
                    }
                    else{
                        ret.push_back(ssid);
                    }
                } break;
                case BSSID:
                    ret.push_back(bssid); break;
                case FLAGS:
                    ret.push_back(flags); break;
                case FREQ:
                    ret.push_back(std::to_string(freq)); break;
                case MODE:
                    ret.push_back(mode); break;
                case RSN_FLAGS:
                    ret.push_back(rsn_flags); break;
                case SEC_TYPE:
                    ret.push_back(sec_type); break;
                case WPA_TYPE:
                    ret.push_back(WPA_type); break;
                case POWER:
                    ret.push_back(std::to_string(power)); break;
                case DEL:
                    ret.push_back("|"); break;
                case EQUAL:
                    ret.push_back("=");break;
                case BRACKET:
                {
                    if(bracket <= 0) {
                        ret.push_back("[");
                        bracket++;
                    }
                    else {
                        ret.push_back("]");
                        bracket--;
                    }
                } break;
                case SPACE:
                    ret.push_back(" "); break;
                case DEF:
                    ret.push_back("-"); break;
                case SLASH:
                    ret.push_back("/"); break;
                case MAX_POWER:
                    ret.push_back("100"); break;
                case MAX_BITRATE:
                    ret.push_back( std::to_string(max_bitrate) ); break;
            }
        }
        return ret;
    }

}

firewolf::wifi::parsing_libnm::large_value
firewolf::wifi::parsing_libnm::get_access_point_full_info_map(std::shared_ptr<NMAccessPoint *> point) {
    std::map<std::string, std::tuple<std::string, GBytes*, int, const char *, setting_type, value_type>> ret;
    ret["SSID"] = std::make_tuple(tools.gbytes_to_string( nm_access_point_get_ssid(*point) ), nm_access_point_get_ssid(*point), -1, NM_SETTING_WIRELESS_SSID, setting_wireless, value_gbyte);
    //ret["SSID"].emplace<4>(2);
    //ret["SSID"].emplace<5>( "ssid" );
    ret["BSSID"] = std::make_tuple((std::string)nm_access_point_get_bssid(*point), nullptr, -1, NM_SETTING_WIRELESS_BSSID, setting_wireless, value_string);
    //ret["SSID"].emplace<4>(1);
    //ret["BSSID"].emplace<5>( "bssid" );

    ret["FREQ"] = std::make_tuple("", nullptr, nm_access_point_get_frequency(*point), nullptr, setting_none, value_int);
    ret["POWER"] = std::make_tuple("", nullptr, nm_access_point_get_strength(*point), nullptr, setting_none, value_int);
    ret["MAX_BITRATE"] = std::make_tuple("", nullptr, nm_access_point_get_max_bitrate(*point), nullptr, setting_none, value_int);
    NM80211ApFlags flags_enum = nm_access_point_get_flags(*point);
    if(flags_enum & NM_802_11_AP_FLAGS_NONE) {
        ret["FLAGS"] = std::make_tuple("none", nullptr, flags_enum, nullptr,  setting_none, value_int);
    }
    else if(flags_enum & NM_802_11_AP_FLAGS_PRIVACY) {
        ret["FLAGS"] = std::make_tuple("privacy", nullptr, flags_enum, nullptr, setting_none, value_int);
    }
    else if(flags_enum & NM_802_11_AP_FLAGS_WPS) {
        ret["FLAGS"] = std::make_tuple("wps", nullptr, flags_enum, nullptr, setting_none, value_int);
    }
    else if(flags_enum & NM_802_11_AP_FLAGS_WPS_PBC) {
        ret["FLAGS"] = std::make_tuple("wps-pbc", nullptr, flags_enum, nullptr, setting_none, value_int);
    }
    else if(flags_enum & NM_802_11_AP_FLAGS_WPS_PIN) {
        ret["FLAGS"] = std::make_tuple("wps-pin", nullptr, flags_enum, nullptr, setting_none, value_int);
    }
    NM80211Mode mode_enum =  nm_access_point_get_mode(*point);
    if (mode_enum & NM_802_11_MODE_ADHOC) {
        ret["MODE"] = std::make_tuple("adhoc", nullptr, mode_enum, nullptr, setting_wireless, value_string);
    }
    else if (mode_enum & NM_802_11_MODE_AP) {
        ret["MODE"] = std::make_tuple("ap", nullptr, mode_enum, nullptr, setting_wireless, value_string);
    }
    else if (mode_enum & NM_802_11_MODE_INFRA) {
        ret["MODE"] = std::make_tuple("infra", nullptr, mode_enum, nullptr, setting_wireless, value_string);
    }
    else if (mode_enum & NM_802_11_MODE_MESH) {
        ret["MODE"] = std::make_tuple("mesh", nullptr, mode_enum, nullptr, setting_wireless, value_string);
    }
    else if (mode_enum & NM_802_11_MODE_UNKNOWN) {
        ret["MODE"] = std::make_tuple("unknow", nullptr, mode_enum, nullptr, setting_wireless, value_string);
    }

    NM80211ApSecurityFlags rsn_flags_enum = nm_access_point_get_rsn_flags(*point);
    if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_PSK ) {
        ret["RSN"] = std::make_tuple("wpa-psk", nullptr, rsn_flags_enum, NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, setting_wireless_secutiry, value_string);
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_802_1X ) {
        ret["RSN"] = std::make_tuple("wpa-802-1x", nullptr, rsn_flags_enum, NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, setting_wireless_secutiry, value_string);
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_EAP_SUITE_B_192 ) {
        ret["RSN"] = std::make_tuple("wpa-eap-suite-b-192", nullptr, rsn_flags_enum, NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, setting_wireless_secutiry, value_string);
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_OWE ) {
        ret["RSN"] = std::make_tuple("owe", nullptr, rsn_flags_enum, NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, setting_wireless_secutiry, value_string);
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_OWE_TM ) {
        ret["RSN"] = std::make_tuple("owe-tm", nullptr, rsn_flags_enum, NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, setting_wireless_secutiry, value_string);
    }
    else if(rsn_flags_enum & NM_802_11_AP_SEC_KEY_MGMT_SAE ) {
        ret["RSN"] = std::make_tuple("sae", nullptr, rsn_flags_enum, NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, setting_wireless_secutiry, value_string);
    }
    else {
        ret["RSN"] = std::make_tuple("none", nullptr, rsn_flags_enum, NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, setting_wireless_secutiry, value_string);
    }
    NM80211ApSecurityFlags sec_type_enum = nm_access_point_get_wpa_flags(*point);
    /*if (sec_type_enum & NM_802_11_AP_SEC_PAIR_WEP40) {
        ret["SEC_TYPE"] = std::make_tuple("pair_wep40", nullptr, sec_type_enum, NM_SETTING_WIRELESS_SECURITY_PAIRWISE, setting_none, value_int);
    }
    else if (sec_type_enum & NM_802_11_AP_SEC_PAIR_WEP104) {
        ret["SEC_TYPE"] = std::make_tuple("pair_wep104", nullptr, sec_type_enum, NM_SETTING_WIRELESS_SECURITY_PAIRWISE, setting_none, value_int);
    }*/
    if (sec_type_enum & NM_802_11_AP_SEC_PAIR_TKIP) {
        ret["SEC_TYPE"] = std::make_tuple("tkip", nullptr, sec_type_enum, NM_SETTING_WIRELESS_SECURITY_PAIRWISE, setting_none, value_int);
    }
    else if (sec_type_enum & NM_802_11_AP_SEC_PAIR_CCMP) {
        ret["SEC_TYPE"] = std::make_tuple("ccmp", nullptr, sec_type_enum, NM_SETTING_WIRELESS_SECURITY_PAIRWISE, setting_none, value_int);
    }
    else if (sec_type_enum & NM_802_11_AP_SEC_GROUP_WEP40) {
        ret["SEC_TYPE"] = std::make_tuple("wep40", nullptr, sec_type_enum, NM_SETTING_WIRELESS_SECURITY_GROUP, setting_none, value_int);
    }
    else if (sec_type_enum & NM_802_11_AP_SEC_GROUP_WEP104) {
        ret["SEC_TYPE"] = std::make_tuple("wep104", nullptr, sec_type_enum, NM_SETTING_WIRELESS_SECURITY_GROUP, setting_none, value_int);
    }
    else if (sec_type_enum & NM_802_11_AP_SEC_GROUP_TKIP) {
        ret["SEC_TYPE"] = std::make_tuple("tkip", nullptr, sec_type_enum, NM_SETTING_WIRELESS_SECURITY_GROUP, setting_none, value_int);
    }
    else if (sec_type_enum & NM_802_11_AP_SEC_GROUP_CCMP) {
        ret["SEC_TYPE"] = std::make_tuple("ccmp", nullptr, sec_type_enum, NM_SETTING_WIRELESS_SECURITY_GROUP, setting_none, value_int);
    }
    else {
        ret["SEC_TYPE"] = std::make_tuple("none", nullptr, sec_type_enum, nullptr, setting_none, value_int);
    }
    if (sec_type_enum & NM_802_11_AP_SEC_PAIR_WEP40 || sec_type_enum & NM_802_11_AP_SEC_PAIR_WEP104 ||
        rsn_flags_enum & NM_802_11_AP_SEC_PAIR_WEP40 || rsn_flags_enum & NM_802_11_AP_SEC_PAIR_WEP104) {
        ret["WPA_TYPE"] = std::make_tuple("wep", nullptr, -1, nullptr, setting_none, value_int);
    } else if (sec_type_enum & NM_802_11_AP_SEC_PAIR_TKIP || sec_type_enum & NM_802_11_AP_SEC_PAIR_CCMP ||
               rsn_flags_enum & NM_802_11_AP_SEC_PAIR_TKIP || rsn_flags_enum & NM_802_11_AP_SEC_PAIR_CCMP) {
        ret["WPA_TYPE"] = std::make_tuple("wpa/wpa2", nullptr, -1, nullptr, setting_none, value_int);
    } else {
        ret["WPA_TYPE"] = std::make_tuple("unknow", nullptr, -1, nullptr, setting_none, value_int);
    }
    return ret;
}

void firewolf::wifi::parsing_libnm::simple_connection(std::string iface, std::string password,
                                                      std::shared_ptr<NMAccessPoint *> point) {
    std::shared_ptr<NMDeviceWifi *> dev = this->get_device_wifi_by_name_smart(iface);
    NMConnection *connection = nm_simple_connection_new();
    NMSetting *s_wireless = nm_setting_wireless_new ();
    NMSetting *s_8021x = nm_setting_802_1x_new();
    NMSetting  *s_sec = nm_setting_wireless_security_new();
    APConf conf;
    conf.get_all(*this, this->tools.nm_access_point_convert_to_smart_point( *point ));
    if(dev) {
        NMConnection *connection = nm_simple_connection_new();
        NMDeviceWifi *device;
        NMAccessPoint *ap;
        NMSetting *s_wireless = nm_setting_wireless_new ();
        NMSetting *s_8021x = nm_setting_802_1x_new();
        NMSetting  *s_sec = nm_setting_wireless_security_new();

        //std::cout << std::get<std::string>(save) << std::endl;


        for(auto d : conf.info) {
            auto val = std::get<setting_type>(d.second);
            if(val != setting_none) {
                //std::cout << std::get<std::string>(d.second) << std::endl;
                switch(val) {
                    case firewolf::wifi::parsing_libnm::setting_wireless: {
                        switch(std::get<value_type>(d.second)) {
                            case firewolf::wifi::parsing_libnm::value_int:
                                g_object_set(s_wireless, std::get<const char *>(d.second), std::get<int>(d.second), nullptr);
                                break;
                            case firewolf::wifi::parsing_libnm::value_gbyte:
                                g_object_set(s_wireless, std::get<const char *>(d.second), std::get<GBytes *>(d.second), nullptr);
                                break;
                            case firewolf::wifi::parsing_libnm::value_string:
                                g_object_set(s_wireless, std::get<const char *>(d.second), std::get<std::string>(d.second).c_str(), nullptr);
                                break;
                        }
                    } break;
                    case firewolf::wifi::parsing_libnm::setting_wireless_secutiry: {
                        switch(std::get<value_type>(d.second)) {
                            case firewolf::wifi::parsing_libnm::value_int:
                                g_object_set(s_sec, std::get<const char *>(d.second), std::get<int>(d.second), nullptr);
                                break;
                            case firewolf::wifi::parsing_libnm::value_gbyte:
                                g_object_set(s_sec, std::get<const char *>(d.second), std::get<GBytes *>(d.second), nullptr);
                                break;
                            case firewolf::wifi::parsing_libnm::value_string:
                                g_object_set(s_sec, std::get<const char *>(d.second), std::get<std::string>(d.second).c_str(), nullptr);
                                break;
                            case value_none:
                                break;
                        }
                    } break;
                    case firewolf::wifi::parsing_libnm::setting_802_11x: {
                        switch(std::get<value_type>(d.second)) {
                            case firewolf::wifi::parsing_libnm::value_int:
                                g_object_set(s_8021x, std::get<const char *>(d.second), std::get<int>(d.second), nullptr);
                                break;
                            case firewolf::wifi::parsing_libnm::value_gbyte:
                                g_object_set(s_8021x, std::get<const char *>(d.second), std::get<GBytes *>(d.second), nullptr);
                                break;
                            case firewolf::wifi::parsing_libnm::value_string:
                                g_object_set(s_8021x, std::get<const char *>(d.second), std::get<std::string>(d.second).c_str(), nullptr);
                                break;
                            case value_none:
                                break;
                        }
                    } break;
                }
            }
        }
        /*g_object_set(s_wireless, NM_SETTING_WIRELESS_BSSID,std::get<GBytes *>(conf.info ["SSID"]), nullptr);
        g_object_set(s_wireless, NM_SETTING_WIRELESS_BSSID,std::get<std::string>(conf.info["BSSID"]).c_str(), nullptr);
        g_object_set(s_sec, NM_SETTING_WIRELESS_SECURITY_KEY_MGMT,"wpa-802-1x", nullptr);
        g_object_set(s_sec, NM_SETTING_WIRELESS_SECURITY_PSK,"412587412587", nullptr);*/
        //std::cout << (conf.dependencies.s_wireless ? "true" : "false") << " " << (conf.dependencies.s_8021x ? "true" : "false") << " " << (conf.dependencies.s_sec ? "true" : "false") << std::endl;
        if(conf.dependencies.s_wireless) {
            nm_connection_add_setting (connection, s_wireless);
        }
        if(conf.dependencies.s_8021x) {
            g_object_set(s_sec, NM_SETTING_802_1X_PASSWORD, password.c_str(), nullptr);
            nm_connection_add_setting(connection, s_8021x);
        }
        if(conf.dependencies.s_sec) {
            g_object_set(s_sec, NM_SETTING_WIRELESS_SECURITY_PSK, password.c_str(), nullptr);
            nm_connection_add_setting(connection, s_sec);
        }
        GMainLoop *loop = g_main_loop_new(nullptr, false);

        std::string pathObj = nm_object_get_path(NM_OBJECT(*point));
        nm_client_add_and_activate_connection_async( this->client, connection, NM_DEVICE(*dev), pathObj.c_str(), nullptr, check_auth, loop);
        //nm_client_activate_connection_async(libnm.client, connection, NM_DEVICE(dev), pathObj.c_str(), cancel, check_auth, loop);
        g_main_loop_run(loop);
        g_main_loop_unref(loop);
    }
    else {
        return;
    }
}

void firewolf::wifi::parsing_libnm::simple_connection(std::shared_ptr<NMDeviceWifi *> dev, std::string password,
                                                      std::shared_ptr<NMAccessPoint *> point) {
    NMConnection *connection = nm_simple_connection_new();
    NMSetting *s_wireless = nm_setting_wireless_new ();
    NMSetting *s_8021x = nm_setting_802_1x_new();
    NMSetting  *s_sec = nm_setting_wireless_security_new();
    APConf conf;
    conf.get_all(*this, this->tools.nm_access_point_convert_to_smart_point( *point ));
    if(dev) {
        NMConnection *connection = nm_simple_connection_new();
        NMDeviceWifi *device;
        NMAccessPoint *ap;
        NMSetting *s_wireless = nm_setting_wireless_new ();
        NMSetting *s_8021x = nm_setting_802_1x_new();
        NMSetting  *s_sec = nm_setting_wireless_security_new();

        //std::cout << std::get<std::string>(save) << std::endl;


        for(auto d : conf.info) {
            auto val = std::get<setting_type>(d.second);
            if(val != setting_none) {
                //std::cout << std::get<std::string>(d.second) << std::endl;
                switch(val) {
                    case firewolf::wifi::parsing_libnm::setting_wireless: {
                        switch(std::get<value_type>(d.second)) {
                            case firewolf::wifi::parsing_libnm::value_int:
                                g_object_set(s_wireless, std::get<const char *>(d.second), std::get<int>(d.second), nullptr);
                                break;
                            case firewolf::wifi::parsing_libnm::value_gbyte:
                                g_object_set(s_wireless, std::get<const char *>(d.second), std::get<GBytes *>(d.second), nullptr);
                                break;
                            case firewolf::wifi::parsing_libnm::value_string:
                                g_object_set(s_wireless, std::get<const char *>(d.second), std::get<std::string>(d.second).c_str(), nullptr);
                                break;
                        }
                    } break;
                    case firewolf::wifi::parsing_libnm::setting_wireless_secutiry: {
                        switch(std::get<value_type>(d.second)) {
                            case firewolf::wifi::parsing_libnm::value_int:
                                g_object_set(s_sec, std::get<const char *>(d.second), std::get<int>(d.second), nullptr);
                                break;
                            case firewolf::wifi::parsing_libnm::value_gbyte:
                                g_object_set(s_sec, std::get<const char *>(d.second), std::get<GBytes *>(d.second), nullptr);
                                break;
                            case firewolf::wifi::parsing_libnm::value_string:
                                g_object_set(s_sec, std::get<const char *>(d.second), std::get<std::string>(d.second).c_str(), nullptr);
                                break;
                            case value_none:
                                break;
                        }
                    } break;
                    case firewolf::wifi::parsing_libnm::setting_802_11x: {
                        switch(std::get<value_type>(d.second)) {
                            case firewolf::wifi::parsing_libnm::value_int:
                                g_object_set(s_8021x, std::get<const char *>(d.second), std::get<int>(d.second), nullptr);
                                break;
                            case firewolf::wifi::parsing_libnm::value_gbyte:
                                g_object_set(s_8021x, std::get<const char *>(d.second), std::get<GBytes *>(d.second), nullptr);
                                break;
                            case firewolf::wifi::parsing_libnm::value_string:
                                g_object_set(s_8021x, std::get<const char *>(d.second), std::get<std::string>(d.second).c_str(), nullptr);
                                break;
                            case value_none:
                                break;
                        }
                    } break;
                }
            }
        }
        /*g_object_set(s_wireless, NM_SETTING_WIRELESS_BSSID,std::get<GBytes *>(conf.info ["SSID"]), nullptr);
        g_object_set(s_wireless, NM_SETTING_WIRELESS_BSSID,std::get<std::string>(conf.info["BSSID"]).c_str(), nullptr);
        g_object_set(s_sec, NM_SETTING_WIRELESS_SECURITY_KEY_MGMT,"wpa-802-1x", nullptr);
        g_object_set(s_sec, NM_SETTING_WIRELESS_SECURITY_PSK,"412587412587", nullptr);*/
        //std::cout << (conf.dependencies.s_wireless ? "true" : "false") << " " << (conf.dependencies.s_8021x ? "true" : "false") << " " << (conf.dependencies.s_sec ? "true" : "false") << std::endl;
        if(conf.dependencies.s_wireless) {
            nm_connection_add_setting (connection, s_wireless);
        }
        if(conf.dependencies.s_8021x) {
            g_object_set(s_sec, NM_SETTING_802_1X_PASSWORD, password.c_str(), nullptr);
            nm_connection_add_setting(connection, s_8021x);
        }
        if(conf.dependencies.s_sec) {
            g_object_set(s_sec, NM_SETTING_WIRELESS_SECURITY_PSK, password.c_str(), nullptr);
            nm_connection_add_setting(connection, s_sec);
        }
        GMainLoop *loop = g_main_loop_new(nullptr, false);

        std::string pathObj = nm_object_get_path(NM_OBJECT(*point));
        nm_client_add_and_activate_connection_async( this->client, connection, NM_DEVICE(*dev), pathObj.c_str(), nullptr, check_auth, loop);
        //nm_client_activate_connection_async(libnm.client, connection, NM_DEVICE(dev), pathObj.c_str(), cancel, check_auth, loop);
        g_main_loop_run(loop);
        g_main_loop_unref(loop);
    }
    else {
        return;
    }
}

void firewolf::wifi::parsing_libnm::spam_connestion() {
    //bruteforse conf
    int len = 5;
    int low_char = 48;
    int max_char = 57;
    int step = 10;
    //conn conf
    std::string iface = "wlp3s0";
    std::string ssid = "ntk-1042v2.0";
    //std::string password(5, static_cast<char>(low_char));
    std::string password = "412587412580";

    auto aps = get_access_points_large(); // access points data
    NMDeviceWifi * dev = get_best_device_wifi();
    //NMDeviceWifi * dev = libnm.get_device_wifi_by_name(iface);

    NMAccessPoint * point;
    try {
        point = std::find_if( aps.begin(), aps.end(), [ssid](auto it) -> bool {
            return std::get<std::string>(it.second["SSID"]) == ssid;
        } )->first;
    } catch (std::runtime_error & e ) { throw std::logic_error("No access point list"); }
    APConf conf;
    conf.get_all(*this, this->tools.nm_access_point_convert_to_smart_point( point ));
    if(dev) {

        NMDeviceWifi *device;
        NMAccessPoint *ap;

        NMSetting *s_actiovate = nm_setting_connection_new();

        //std::cout << std::get<std::string>(save) << std::endl;


        /*g_object_set(s_wireless, NM_SETTING_WIRELESS_BSSID,std::get<GBytes *>(conf.info ["SSID"]), nullptr);
        g_object_set(s_wireless, NM_SETTING_WIRELESS_BSSID,std::get<std::string>(conf.info["BSSID"]).c_str(), nullptr);
        g_object_set(s_sec, NM_SETTING_WIRELESS_SECURITY_KEY_MGMT,"wpa-802-1x", nullptr);
        g_object_set(s_sec, NM_SETTING_WIRELESS_SECURITY_PSK,"412587412587", nullptr);*/
        //std::cout << (conf.dependencies.s_wireless ? "true" : "false") << " " << (conf.dependencies.s_8021x ? "true" : "false") << " " << (conf.dependencies.s_sec ? "true" : "false") << std::endl;
        //      NM_SETTINGS_CONNECTION_FLAG_UNSAVED
        //g_object_set(s_actiovate, NM_SETTING_CONNECTION_SETTING_NAME, ssid.c_str(), nullptr);




        //std::cout << "verify: " << nm_connection_verify(connection, &this->error) << std::endl;

        std::string pathObj = point ? nm_object_get_path(NM_OBJECT(point)) : "/";
        //nm_client_add_connection_async(this->client, connection, true, nullptr, check_add, loop);



        //nm_client_add_and_activate_connection_async( this->client, connection, NM_DEVICE(dev), pathObj.c_str(), nullptr, check_auth, loop);
        auto connect = [this, dev, conf, &point]( std::string password) -> bool {
            NMConnection *connection = nm_simple_connection_new();
            signal(SIGTRAP, firewolf::wifi::parsing_libnm::error_dumper);
            signal(SIGSEGV, firewolf::wifi::parsing_libnm::error_dumper);
            GMainLoop *loop = g_main_loop_new(nullptr, false);
            bool verified = false;
            specific_data user_data = {
                    std::move(std::make_shared<GMainLoop *>( loop)),
                    std::move(std::make_shared<NMDeviceWifi *>( dev)),
                    std::move(std::make_shared<NMAccessPoint *>( point)),
                    std::move(std::make_shared<NMConnection *>( connection)),
                    password,
                    std::move(std::make_shared<bool>(verified))
            };

            GVariantBuilder * builder = g_variant_builder_new (G_VARIANT_TYPE_VARDICT);
            g_variant_builder_add (builder, "{sv}", "bind-activation", g_variant_new_string ("dbus-client"));
            g_variant_builder_add (builder, "{sv}", "persist", g_variant_new_string ("volatile"));
            NMSetting *s_wireless = nm_setting_wireless_new ();
            NMSetting *s_8021x = nm_setting_802_1x_new();
            NMSetting  *s_sec = nm_setting_wireless_security_new();
            for (auto d: conf.info) {
                auto val = std::get<setting_type>(d.second);
                if (val != setting_none) {
                    //std::cout << std::get<std::string>(d.second) << std::endl;
                    switch (val) {
                        case firewolf::wifi::parsing_libnm::setting_wireless: {
                            switch (std::get<value_type>(d.second)) {
                                case firewolf::wifi::parsing_libnm::value_int:
                                    g_object_set(s_wireless, std::get<const char *>(d.second),
                                                 std::get<int>(d.second), nullptr);
                                    break;
                                case firewolf::wifi::parsing_libnm::value_gbyte:
                                    g_object_set(s_wireless, std::get<const char *>(d.second),
                                                 std::get<GBytes *>(d.second), nullptr);
                                    break;
                                case firewolf::wifi::parsing_libnm::value_string:
                                    g_object_set(s_wireless, std::get<const char *>(d.second),
                                                 std::get<std::string>(d.second).c_str(), nullptr);
                                    break;
                            }
                        }
                            break;
                        case firewolf::wifi::parsing_libnm::setting_wireless_secutiry: {
                            switch (std::get<value_type>(d.second)) {
                                case firewolf::wifi::parsing_libnm::value_int:
                                    g_object_set(s_sec, std::get<const char *>(d.second),
                                                 std::get<int>(d.second), nullptr);
                                    break;
                                case firewolf::wifi::parsing_libnm::value_gbyte:
                                    g_object_set(s_sec, std::get<const char *>(d.second),
                                                 std::get<GBytes *>(d.second), nullptr);
                                    break;
                                case firewolf::wifi::parsing_libnm::value_string:
                                    g_object_set(s_sec, std::get<const char *>(d.second),
                                                 std::get<std::string>(d.second).c_str(), nullptr);
                                    break;
                                case value_none:
                                    break;
                            }
                        }
                            break;
                        case firewolf::wifi::parsing_libnm::setting_802_11x: {
                            switch (std::get<value_type>(d.second)) {
                                case firewolf::wifi::parsing_libnm::value_int:
                                    g_object_set(s_8021x, std::get<const char *>(d.second),
                                                 std::get<int>(d.second), nullptr);
                                    break;
                                case firewolf::wifi::parsing_libnm::value_gbyte:
                                    g_object_set(s_8021x, std::get<const char *>(d.second),
                                                 std::get<GBytes *>(d.second), nullptr);
                                    break;
                                case firewolf::wifi::parsing_libnm::value_string:
                                    g_object_set(s_8021x, std::get<const char *>(d.second),
                                                 std::get<std::string>(d.second).c_str(), nullptr);
                                    break;
                                case value_none:
                                    break;
                            }
                        }
                            break;
                    }
                }
            }
            try {
                if (conf.dependencies.s_wireless) {
                    nm_connection_add_setting(connection, s_wireless);
                }
                if (conf.dependencies.s_8021x) {
                    g_object_set(s_sec, NM_SETTING_802_1X_PASSWORD, password.c_str(), nullptr);
                    nm_connection_add_setting(connection, s_8021x);
                }
                if (conf.dependencies.s_sec) {
                    g_object_set(s_sec, NM_SETTING_WIRELESS_SECURITY_PSK, password.c_str(), nullptr);
                    nm_connection_add_setting(connection, s_sec);
                }

                GVariant *options = g_variant_builder_end (builder);

                output_all_device_states(NM_DEVICE(dev));


                nm_client_add_and_activate_connection2(
                        this->client, connection, NM_DEVICE(dev),
                        "/", options, nullptr, check_auth2, static_cast<void*>(&user_data) );
                g_main_loop_run(loop);

                auto state = nm_device_get_state(NM_DEVICE(dev));
                std::cout << "Returned : " << *user_data.verified << std::endl;

                nm_client_deactivate_connection(client, nm_device_get_active_connection(NM_DEVICE(dev)),
                                                nullptr, nullptr);
                g_main_loop_unref(loop);
                g_object_unref(connection);
            }
            catch (std::runtime_error & e) {  }
            return *user_data.verified;
        };
        bool stop = false;
        while(!stop) {
            int check = 0;
            for(auto iter = password.end()-1; iter != password.begin()-1; iter--) {
                if(std::distance(password.begin(), iter) -1 > -1 ) {
                    if(*iter > max_char) {
                        *iter = low_char;
                        *(iter-1) += 1;
                        check++;
                    }
                }
                else {
                    if(*iter > max_char && check == len-1) {
                        stop = true;
                    }
                }
            }
            if(!stop) {
                std::cout << password << std::endl;
                stop = connect(password);
            }
            if(password == "412587412590") {
                break;
            }
            *(password.end()-1) += 1;
            sleep(0);
        }


    }
    else {
        return;
    }
    /*bool stop = false;
    while(!stop) {
        int check = 0;
        for(auto iter = password.end()-1; iter != password.begin()-1; iter--) {
            if(std::distance(password.begin(), iter) -1 > -1 ) {
                if(*iter > max_char) {
                    *iter = low_char;
                    *(iter-1) += 1;
                    check++;
                }
            }
            else {
                if(*iter > max_char && check == len-1) {
                    stop = true;
                }
            }
        }
        if(!stop) {
            std::cout << password << std::endl;
        }
        *(password.end()-1) += 1;
        sleep(0);
    }*/

    //std::string brute = "00000";

}


void firewolf::wifi::parsing_libnm::APConf::get_all(firewolf::wifi::parsing_libnm &libnm,
                                                    std::shared_ptr<_NMAccessPoint *> point) {
    this->info = libnm.get_access_point_full_info_map(point);
    if(std::get<std::string>(this->info["RSN"]) == "wpa-psk" ) {
        dependencies = {true, false, true};
    }
    else if(std::get<std::string>(this->info["RSN"]) == "wpa-802-1x" ) {
        dependencies = {true, true, false};
    }
    else if(std::get<std::string>(this->info["RSN"]) == "none" ) {
        dependencies = {true, false, false};
    }
}

void firewolf::wifi::parsing_libnm::output_all_device_states(NMDevice *dev)  {
    std::cout << "=======================================================\n";
    std::cout << "                      device info\n";
    std::cout << "=======================================================\n";
    std::cout << "Managed: " << (nm_device_get_managed(dev) ? "true" : "false") << std::endl;
    NMDeviceCapabilities dev_cap = nm_device_get_capabilities(dev);
    std::cout << "Capabilities: " << this->get_device_capabilities(dev_cap);

    auto state_reason = nm_device_get_state_reason( dev );
    std::cout << "State reason: ";
    switch(state_reason) {
        case NM_DEVICE_STATE_REASON_NONE:
            std::cout << "NM_DEVICE_STATE_REASON_NONE" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_UNKNOWN:
            std::cout << "NM_DEVICE_STATE_REASON_UNKNOWN" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_NOW_MANAGED:
            std::cout << "NM_DEVICE_STATE_REASON_NOW_MANAGED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_NOW_UNMANAGED:
            std::cout << "NM_DEVICE_STATE_REASON_NOW_UNMANAGED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_CONFIG_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_CONFIG_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_IP_CONFIG_UNAVAILABLE:
            std::cout << "NM_DEVICE_STATE_REASON_IP_CONFIG_UNAVAILABLE" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_IP_CONFIG_EXPIRED:
            std::cout << "NM_DEVICE_STATE_REASON_IP_CONFIG_EXPIRED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_NO_SECRETS:
            std::cout << "NM_DEVICE_STATE_REASON_NO_SECRETS" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_SUPPLICANT_DISCONNECT:
            std::cout << "NM_DEVICE_STATE_REASON_SUPPLICANT_DISCONNECT" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_SUPPLICANT_CONFIG_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_SUPPLICANT_CONFIG_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_SUPPLICANT_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_SUPPLICANT_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_SUPPLICANT_TIMEOUT:
            std::cout << "NM_DEVICE_STATE_REASON_SUPPLICANT_TIMEOUT" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_PPP_START_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_PPP_START_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_PPP_DISCONNECT:
            std::cout << "NM_DEVICE_STATE_REASON_PPP_DISCONNECT" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_PPP_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_PPP_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_DHCP_START_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_DHCP_START_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_DHCP_ERROR:
            std::cout << "NM_DEVICE_STATE_REASON_DHCP_ERROR" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_DHCP_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_DHCP_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_SHARED_START_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_SHARED_START_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_SHARED_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_SHARED_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_AUTOIP_START_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_AUTOIP_START_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_AUTOIP_ERROR:
            std::cout << "NM_DEVICE_STATE_REASON_AUTOIP_ERROR" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_AUTOIP_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_AUTOIP_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_MODEM_BUSY:
            std::cout << "NM_DEVICE_STATE_REASON_MODEM_BUSY" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_MODEM_NO_DIAL_TONE:
            std::cout << "NM_DEVICE_STATE_REASON_MODEM_NO_DIAL_TONE" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_MODEM_NO_CARRIER:
            std::cout << "NM_DEVICE_STATE_REASON_MODEM_NO_CARRIER" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_MODEM_DIAL_TIMEOUT:
            std::cout << "NM_DEVICE_STATE_REASON_MODEM_DIAL_TIMEOUT" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_MODEM_DIAL_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_MODEM_DIAL_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_MODEM_INIT_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_MODEM_INIT_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_GSM_APN_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_GSM_APN_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_GSM_REGISTRATION_NOT_SEARCHING:
            std::cout << "NM_DEVICE_STATE_REASON_GSM_REGISTRATION_NOT_SEARCHING" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_GSM_REGISTRATION_DENIED:
            std::cout << "NM_DEVICE_STATE_REASON_GSM_REGISTRATION_DENIED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_GSM_REGISTRATION_TIMEOUT:
            std::cout << "NM_DEVICE_STATE_REASON_GSM_REGISTRATION_TIMEOUT" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_GSM_REGISTRATION_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_GSM_REGISTRATION_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_GSM_PIN_CHECK_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_GSM_PIN_CHECK_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_FIRMWARE_MISSING:
            std::cout << "NM_DEVICE_STATE_REASON_FIRMWARE_MISSING" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_REMOVED:
            std::cout << "NM_DEVICE_STATE_REASON_REMOVED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_SLEEPING:
            std::cout << "NM_DEVICE_STATE_REASON_SLEEPING" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_CONNECTION_REMOVED:
            std::cout << "NM_DEVICE_STATE_REASON_CONNECTION_REMOVED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_USER_REQUESTED:
            std::cout << "NM_DEVICE_STATE_REASON_USER_REQUESTED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_CARRIER:
            std::cout << "NM_DEVICE_STATE_REASON_CARRIER" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_CONNECTION_ASSUMED:
            std::cout << "NM_DEVICE_STATE_REASON_CONNECTION_ASSUMED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_SUPPLICANT_AVAILABLE:
            std::cout << "NM_DEVICE_STATE_REASON_SUPPLICANT_AVAILABLE" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_MODEM_NOT_FOUND:
            std::cout << "NM_DEVICE_STATE_REASON_MODEM_NOT_FOUND" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_BT_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_BT_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_GSM_SIM_NOT_INSERTED:
            std::cout << "NM_DEVICE_STATE_REASON_GSM_SIM_NOT_INSERTED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_GSM_SIM_PIN_REQUIRED:
            std::cout << "NM_DEVICE_STATE_REASON_GSM_SIM_PIN_REQUIRED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_GSM_SIM_PUK_REQUIRED:
            std::cout << "NM_DEVICE_STATE_REASON_GSM_SIM_PUK_REQUIRED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_GSM_SIM_WRONG:
            std::cout << "NM_DEVICE_STATE_REASON_GSM_SIM_WRONG" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_INFINIBAND_MODE:
            std::cout << "NM_DEVICE_STATE_REASON_INFINIBAND_MODE" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_DEPENDENCY_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_DEPENDENCY_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_BR2684_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_BR2684_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_MODEM_MANAGER_UNAVAILABLE:
            std::cout << "NM_DEVICE_STATE_REASON_MODEM_MANAGER_UNAVAILABLE" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_SSID_NOT_FOUND:
            std::cout << "NM_DEVICE_STATE_REASON_SSID_NOT_FOUND" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_SECONDARY_CONNECTION_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_SECONDARY_CONNECTION_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_DCB_FCOE_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_DCB_FCOE_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_TEAMD_CONTROL_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_TEAMD_CONTROL_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_MODEM_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_MODEM_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_MODEM_AVAILABLE:
            std::cout << "NM_DEVICE_STATE_REASON_MODEM_AVAILABLE" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_SIM_PIN_INCORRECT:
            std::cout << "NM_DEVICE_STATE_REASON_SIM_PIN_INCORRECT" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_NEW_ACTIVATION:
            std::cout << "NM_DEVICE_STATE_REASON_NEW_ACTIVATION" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_PARENT_CHANGED:
            std::cout << "NM_DEVICE_STATE_REASON_PARENT_CHANGED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_PARENT_MANAGED_CHANGED:
            std::cout << "NM_DEVICE_STATE_REASON_PARENT_MANAGED_CHANGED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_OVSDB_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_OVSDB_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_IP_ADDRESS_DUPLICATE:
            std::cout << "NM_DEVICE_STATE_REASON_IP_ADDRESS_DUPLICATE" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_IP_METHOD_UNSUPPORTED:
            std::cout << "NM_DEVICE_STATE_REASON_IP_METHOD_UNSUPPORTED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_SRIOV_CONFIGURATION_FAILED:
            std::cout << "NM_DEVICE_STATE_REASON_SRIOV_CONFIGURATION_FAILED" << std::endl;
            break;
        case NM_DEVICE_STATE_REASON_PEER_NOT_FOUND:
            std::cout << "NM_DEVICE_STATE_REASON_PEER_NOT_FOUND" << std::endl;
            break;
    }
    auto device_type = nm_device_get_device_type(dev);
    std::cout << "Device type: " << this->get_device_type(device_type);
}

std::vector<std::string> firewolf::wifi::parsing_libnm::get_name_devices() {
    auto row = get_devices();
    std::vector<std::string> mass;
    for(auto data : row) {
        mass.emplace_back(nm_device_get_iface(static_cast<NMDevice *>(data)) );
    }
    return mass;
}
std::unordered_map<std::string,std::unordered_map<std::string, std::string>> firewolf::wifi::parsing_libnm::get_data_devices() {
    auto row = get_devices();
    std::unordered_map<std::string,std::unordered_map<std::string, std::string>> mass;
    for(auto data : row) {
        //static_cast<NMDevice *>(data)
        mass[std::string(nm_device_get_iface(static_cast<NMDevice *>(data)))] = this->get_device_info(static_cast<NMDevice *>(data));
    }

    return mass;
}
std::unordered_map<std::string, std::string> firewolf::wifi::parsing_libnm::get_device_info(NMDevice * dev) {
    std::string iface;
    try {
        iface = (nm_device_get_iface(dev) );
    } catch (const std::exception& e) {}
    std::string driver;
    try {
        driver = std::string(nm_device_get_driver(dev));
    } catch (const std::exception& e) {}
    std::string bitrate( std::to_string( nm_device_wifi_get_bitrate(NM_DEVICE_WIFI(dev)) / 1000 ) );
    bool ap = nm_device_get_state (dev) == NM_DEVICE_STATE_ACTIVATED;
    std::string active = (ap) ? "true" : "false";
    std::string description;
    try {
        description= nm_device_get_description(dev);
    } catch (const std::exception& e) {}
    std::string type = this->get_device_type(nm_device_get_device_type(dev));
    std::string capabilities = this->get_device_capabilities(nm_device_get_capabilities(dev));
    std::string name_active;
    if(ap) {
        auto active_ap = nm_device_wifi_get_active_access_point (NM_DEVICE_WIFI (dev));
        auto active_ssid = nm_access_point_get_ssid (active_ap);
        if(active_ssid) {
            name_active = std::string(nm_utils_ssid_to_utf8(static_cast<const guint8 *>(g_bytes_get_data (active_ssid, NULL)),
                                                             g_bytes_get_size (active_ssid)));
        }
    }
    return {{"DEVICE", iface}, {"TYPE", type}, {"CAP", capabilities}, {"DRIVER", driver}, {"BITRATE", bitrate}, {"ACTIVE", active},
            {"ACTIVE SSID", name_active}, {"DESCRIPTION", description}};
}

std::string firewolf::wifi::parsing_libnm::get_device_type(NMDeviceType type) {
    switch(type) {
        case NM_DEVICE_TYPE_UNKNOWN:
            return "UNKNOWN";
        case NM_DEVICE_TYPE_ETHERNET:
            return "ETHERNET";
        case NM_DEVICE_TYPE_WIFI:
            return "WIFI";
        case NM_DEVICE_TYPE_UNUSED1:
            return "UNUSED1";
        case NM_DEVICE_TYPE_UNUSED2:
            return "UNUSED2";
        case NM_DEVICE_TYPE_BT:
            return "BT";
        case NM_DEVICE_TYPE_OLPC_MESH:
            return "OLPC_MESH";
        case NM_DEVICE_TYPE_WIMAX:
            return "WIMAX";
        case NM_DEVICE_TYPE_MODEM:
            return "MODEM";
        case NM_DEVICE_TYPE_INFINIBAND:
            return "INFINIBAND";
        case NM_DEVICE_TYPE_BOND:
            return "BOND";
        case NM_DEVICE_TYPE_VLAN:
            return "VLAN";
        case NM_DEVICE_TYPE_ADSL:
            return "ADSL";
        case NM_DEVICE_TYPE_BRIDGE:
            return "BRIDGE";
        case NM_DEVICE_TYPE_GENERIC:
            return "GENERIC";
        case NM_DEVICE_TYPE_TEAM:
            return "TEAM";
        case NM_DEVICE_TYPE_TUN:
            return "TUN";
        case NM_DEVICE_TYPE_IP_TUNNEL:
            return "IP_TUNNEL";
        case NM_DEVICE_TYPE_MACVLAN:
            return "MACVLAN";
        case NM_DEVICE_TYPE_VXLAN:
            return "VXLAN";
        case NM_DEVICE_TYPE_VETH:
            return "VETH";
        case NM_DEVICE_TYPE_MACSEC:
            return "MACSEC";
        case NM_DEVICE_TYPE_DUMMY:
            return "DUMMY";
        case NM_DEVICE_TYPE_PPP:
            return "PPP";
        case NM_DEVICE_TYPE_OVS_INTERFACE:
            return "OVS_INTERFACE";
        case NM_DEVICE_TYPE_OVS_PORT:
            return "OVS_PORT";
        case NM_DEVICE_TYPE_OVS_BRIDGE:
            return "OVS_BRIDGE";
        case NM_DEVICE_TYPE_WPAN:
            return "WPAN";
        case NM_DEVICE_TYPE_6LOWPAN:
            return "6LOWPAN";
        case NM_DEVICE_TYPE_WIREGUARD:
            return "WIREGUARD";
        case NM_DEVICE_TYPE_WIFI_P2P:
            return "WIFI_P2P";
        case NM_DEVICE_TYPE_VRF:
            return "VRF";
        case NM_DEVICE_TYPE_LOOPBACK:
            return "LOOPBACK";
        default:
            return "UNKNOWN";
    }
}

std::string firewolf::wifi::parsing_libnm::get_device_capabilities(NMDeviceCapabilities capabilities) {
    std::string res;
    if(capabilities & NM_DEVICE_CAP_NONE)
        res += "CAP_NONE";
    if (capabilities & NM_DEVICE_CAP_NM_SUPPORTED)
        res += (res.empty() ? "" : "|") + std::string("NM_SUPPORTED");
    if (capabilities & NM_DEVICE_CAP_CARRIER_DETECT)
        res += (res.empty() ? "" : "|") + std::string("CARRIER_DETECT");
    if (capabilities & NM_DEVICE_CAP_IS_SOFTWARE)
        res += (res.empty() ? "" : "|") + std::string("IS_SOFTWARE");
    if (capabilities & NM_DEVICE_CAP_SRIOV)
        res += (res.empty() ? "" : "|") + std::string("SRIOV");
    return res;
}


