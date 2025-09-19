// simple_arg.hpp
#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

/**
 * \ingroup utility
 * @brief A simple command-line argument parser.
 * 
 */
struct SimpleArg {
    /**
     * @brief An option (named or positional).
     * 
     */
    struct Opt {
        /**
         * @brief Short name (e.g. 'h' for -h).
         * 
         */
        char                      s{};
        /**
         * @brief Long name (e.g. "help" for --help).
         * 
         */
        std::string               l;
        /**
         * @brief Description of the option.
         * 
         */
        std::string               desc;
        /**
         * @brief Default value as string (empty if none).
         * 
         */
        std::string               default_str;
        /**
         * @brief Type name (e.g. "int", "float", "string", "enum", "flag").
         * 
         */
        std::string               type_name;
        /**
         * @brief Whether the option takes a value (true) or is a flag (false).
         * 
         */
        bool                      takes_val{};
        /**
         * @brief Whether the option is required (true) or optional (false).
         * 
         */
        bool                      required{};
        /**
         * @brief Allowed choices (for enum types).
         * 
         */
        std::vector<std::string>  choices;
    };

    /**
     * @brief The name of the program (usually argv[0]).
     * 
     */
    std::string                    prog;

    /**
     * @brief The list of named options.
     * 
     */
    std::vector<Opt>               opts;

    /**
     * @brief The list of positional options.
     * 
     */
    std::vector<Opt>               positionals;

    /**
     * @brief The parsed values.
     * 
     */
    std::map<std::string,std::string> vals;

    /**
     * @brief The parsed flags.
     * 
     */
    std::map<std::string,bool>        flags;

    /**
     * @brief The list of errors encountered during parsing.
     * 
     */
    std::vector<std::string>          errs;

    /**
     * @brief Construct a new Simple Arg object
     * 
     * @param p Name of the program (usually argv[0]).
     */
    SimpleArg(std::string p) : prog(std::move(p)) {
        add<bool>('h', "help", "show this help message");
    }

    // — register named args of any type T —
    /**
     * @brief Add a named argument.
     * 
     * @tparam T Type of the argument. Only bool, int, float, and std::string are supported.
     * @param s The short name (e.g. 'h' for -h).
     * @param l The long name (e.g. "help" for --help).
     * @param d Description of the argument.
     * @param req Whether the argument is required (true) or optional (false).
     * @param def Default value (only if req is false).
     * @param choices Allowed choices (only for enum types).
     */
    template<typename T>
    void add(char s,
             std::string l,
             std::string d,
             bool req = false,
             T def = T{},
             std::vector<std::string> choices = {})
    {
        Opt o;
        o.s        = s;
        o.l        = std::move(l);
        o.desc     = std::move(d);
        o.required = req;
        o.choices  = std::move(choices);

        if constexpr(std::is_same_v<T,bool>) {
            o.takes_val = false;
            o.type_name = "flag";
            flags[o.l]  = false;
        } else {
            o.takes_val = true;
            if (!req) {
                if constexpr(std::is_same_v<T,std::string>) o.default_str = def;
                else                                         o.default_str = std::to_string(def);
                if (!o.default_str.empty())
                    vals[o.l] = o.default_str;
            }
            if (!o.choices.empty())                        o.type_name = "enum";
            else if constexpr(std::is_integral_v<T>)       o.type_name = "int";
            else if constexpr(std::is_floating_point_v<T>) o.type_name = "float";
            else                                           o.type_name = "string";
        }
        opts.push_back(std::move(o));
    }

    // — register positional args (always required) —
    /**
     * @brief Add a positional argument.
     * 
     * @tparam T Type of the argument. Only int, float, and std::string are supported.
     * @param l The name of the positional argument (for help messages).
     * @param d Description of the argument.
     * @param choices Allowed choices (only for enum types).
     */
    template<typename T>
    void addPositional(std::string l,
                       std::string d,
                       std::vector<std::string> choices = {})
    {
        Opt o;
        o.s          = '\0';
        o.l          = std::move(l);
        o.desc       = std::move(d);
        o.takes_val  = true;
        o.required   = true;
        o.choices    = std::move(choices);

        if (!o.choices.empty())                        o.type_name = "enum";
        else if constexpr(std::is_integral_v<T>)       o.type_name = "int";
        else if constexpr(std::is_floating_point_v<T>) o.type_name = "float";
        else                                           o.type_name = "string";

        positionals.push_back(std::move(o));
    }

    // — parse everything, stopping at first error —
    /**
     * @brief Parse the command-line arguments.
     * 
     * @param argc The number of arguments (from main).
     * @param argv The argument values (from main).
     * @return true if parsing was successful, false otherwise.
     */
    bool parse(int argc, char** argv) {
        std::vector<std::string> raw_pos;

        for (int i = 1; i < argc; ++i) {
            std::string a = argv[i];

            // — long form: --name or --name=val —
            if (a.rfind("--",0) == 0) {
                auto eq = a.find('=');
                std::string name = a.substr(2, (eq==std::string::npos? a.size()-2: eq-2));
                std::string val;
                if (eq != std::string::npos) val = a.substr(eq+1);

                auto it = std::find_if(opts.begin(), opts.end(),
                    [&](auto &o){ return o.l == name; });
                if (it == opts.end()) {
                    errs.push_back("unknown option --" + name);
                    return false;
                }
                if (!it->takes_val) {
                    flags[name] = true;
                } else {
                    if (eq == std::string::npos) {
                        if (i+1 < argc) val = argv[++i];
                        else {
                            errs.push_back("--" + name + " requires a value");
                            return false;
                        }
                    }
                    if (!store(*it, name, val)) return false;
                }
            }

            // — short form: -x, -xyz, -xval, -xyval, etc. —
            else if (a.size() > 1 && a[0]=='-' && a[1] != '-') {
                // peel off one short‐opt at a time:
                for (size_t pos = 1; pos < a.size(); ) {
                    char s = a[pos++];
                    auto it = std::find_if(opts.begin(), opts.end(),
                        [&](auto &o){ return o.s == s; });
                    if (it == opts.end()) {
                        errs.push_back(std::string("unknown option -") + s);
                        return false;
                    }
                    if (!it->takes_val) {
                        flags[it->l] = true;
                        continue;
                    }
                    // this option takes a value:
                    std::string val;
                    if (pos < a.size()) {
                        // e.g. "-n3" or "-oout.txt"
                        val = a.substr(pos);
                    } else {
                        // e.g. "-n 3"
                        if (i+1 < argc) val = argv[++i];
                        else {
                            errs.push_back(std::string("-") + s + " requires a value");
                            return false;
                        }
                    }
                    if (!store(*it, it->l, val)) return false;
                    break;  // stop parsing this arg
                }
            }

            // — positional —
            else {
                raw_pos.push_back(a);
            }
        }

        // if user asked for help, skip missing checks
        if (get<bool>("help")) return true;

        // required named
        for (auto &o : opts) {
            if (o.takes_val && o.required && !vals.count(o.l)) {
                errs.push_back("missing required --" + o.l);
                return false;
            }
        }
        // positional count
        if (raw_pos.size() < positionals.size()) {
            errs.push_back("missing positional <" + positionals[raw_pos.size()].l + ">");
            return false;
        }
        if (raw_pos.size() > positionals.size()) {
            errs.push_back("unexpected argument '" + raw_pos[positionals.size()] + "'");
            return false;
        }
        // store positional (with type‐check)
        for (size_t i = 0; i < positionals.size(); ++i) {
            if (!store(positionals[i], positionals[i].l, raw_pos[i]))
                return false;
        }

        return true;
    }

    // — single get<T> for named, plus help()/errors()/errorMsg() —
    /**
     * @brief Get the value of a named argument.
     * 
     * @tparam T Type of the argument. Only bool, int, float, and std::string are supported.
     * @param name The long name of the argument (e.g. "help" for --help).
     * @return T The value of the argument.
     */
    template<typename T>
    T get(const std::string &name) const {
        if constexpr(std::is_same_v<T,bool>) {
            auto f = flags.find(name);
            return (f != flags.end()) && f->second;
        }
        auto v = vals.find(name);
        if (v == vals.end())
            throw std::runtime_error("no value for " + name);
        const auto &s = v->second;
        if constexpr(std::is_same_v<T,std::string>) {
            return s;
        }
        else if constexpr(std::is_integral_v<T>) {
            if constexpr(std::is_signed_v<T>)   return static_cast<T>(std::stoll(s));
            else                                 return static_cast<T>(std::stoull(s));
        }
        else if constexpr(std::is_floating_point_v<T>) {
            return static_cast<T>(std::stod(s));
        }
        else static_assert(!sizeof(T), "unsupported get<T>");
    }
    
    /**
     * @brief Check if help was requested.
     * 
     * @return true if help was requested, false otherwise.
     */
    bool        help()   const { return get<bool>("help"); }

    /**
     * @brief Check if there were any errors during parsing.
     * 
     * @return true if there were errors, false otherwise.
     */
    bool        errors() const { return !errs.empty(); }

    /**
     * @brief Get the error message.
     * 
     * @return std::string The error message, or an empty string if there were no errors.
     */
    std::string errorMsg() const {
        return errs.empty() ? "" : std::string("error: ") + errs[0];
    }

    // — get positional by index —
    /**
     * @brief Get the Positional object
     * @tparam T Type of the positional argument. Only int, float, and std::string are supported.
     * @param idx 
     * @return T The value of the positional argument at index idx.
     */
    template<typename T>
    T getPositional(size_t idx) const {
        if (idx >= positionals.size())
            throw std::out_of_range("no positional at index " + std::to_string(idx));
        return get<T>(positionals[idx].l);
    }

    // — pretty‐print help —
    /**
     * @brief Print the help message.
     * 
     * @param os The output stream to print to (default: std::cout).
     */
    void printHelp(std::ostream &os = std::cout) const {
        os << "Usage: " << prog << " [options]";
        for (auto &p : positionals)
            os << " <" << p.l << ">";
        os << "\n\nOptions:\n";

        // calc width
        size_t W = 0;
        for (auto &o : opts) {
            std::ostringstream t;
            t << "  " 
              << (o.s ? "-" + std::string(1,o.s) + ", " : "    ")
              << "--" << o.l
              << (o.takes_val ? "<"+o.type_name+">" : "");
            W = std::max(W, t.str().size());
        }

        // named
        for (auto &o : opts) {
            std::ostringstream nm;
            nm << "  " 
               << (o.s? "-" + std::string(1,o.s)+", " : "    ")
               << "--" << o.l
               << (o.takes_val? "<"+o.type_name+">" : "");
            os << nm.str()
               << std::string(W - nm.str().size() + 2, ' ')
               << o.desc;
            if (o.takes_val && o.required) os << " (required)";
            if (!o.choices.empty()) {
                os << " {";
                for (size_t i=0; i<o.choices.size(); ++i) {
                    os << o.choices[i]
                       << (i+1<o.choices.size()? ", " : "");
                }
                os << "}";
            }
            if (!o.default_str.empty() && !o.required)
                os << " (default: " << o.default_str << ")";
            os << "\n";
        }

        // positionals
        if (!positionals.empty()) {
            os << "\nPositional arguments:\n";
            for (auto &p : positionals) {
                std::ostringstream nm;
                nm << "  " << p.l << "<" << p.type_name << ">";
                os << nm.str()
                   << std::string(W + 2 - nm.str().size(), ' ')
                   << p.desc << " (required)\n";
            }
        }
    }

private:
    // store + full type‐check
    bool store(const Opt &o, const std::string &key, const std::string &val) {
        if (o.type_name == "int") {
            try { std::stoll(val); }
            catch(...) {
                errs.push_back("invalid integer for --" + key + ": '" + val + "'");
                return false;
            }
        }
        else if (o.type_name == "float") {
            try { std::stod(val); }
            catch(...) {
                errs.push_back("invalid float for --" + key + ": '" + val + "'");
                return false;
            }
        }
        else if (o.type_name == "enum") {
            if (std::find(o.choices.begin(), o.choices.end(), val) == o.choices.end()) {
                errs.push_back("invalid value for --" + key + ": '" + val + "'");
                return false;
            }
        }
        // string always OK
        vals[key] = val;
        return true;
    }
};
