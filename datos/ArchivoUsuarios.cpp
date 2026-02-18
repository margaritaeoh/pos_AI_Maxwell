#include "ArchivoUsuarios.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace pos {

ArchivoUsuarios::ArchivoUsuarios(const std::string& ruta)
    : rutaArchivo(ruta) {}

bool ArchivoUsuarios::cargar(std::vector<Usuario>& usuarios) {
    usuarios.clear();

    std::ifstream in(rutaArchivo);
    if (!in.is_open()) return false;

    std::string linea;
    std::getline(in, linea); // encabezado

    while (std::getline(in, linea)) {
        std::stringstream ss(linea);
        std::string campo;
        Usuario u{};

        std::getline(ss, u.username, ',');
        std::getline(ss, u.passwordHash, ',');
        std::getline(ss, campo, ',');
        
        if (campo == "Administrador")
            u.rol = RolUsuario::Administrador;
        else
            u.rol = RolUsuario::Usuario;
        
        // Read requierePassword field (default to true if not present)
        std::string passwordReqStr;
        if (std::getline(ss, passwordReqStr, ',')) {
            u.requierePassword = (passwordReqStr == "1" || passwordReqStr == "true");
        } else {
            u.requierePassword = true;
        }

        usuarios.push_back(u);
    }

    return true;
}

bool ArchivoUsuarios::guardar(const std::vector<Usuario>& usuarios) {
    std::ofstream out(rutaArchivo);
    if (!out.is_open()) return false;

    out << "username,passwordHash,rol,requierePassword\n";

    for (const auto& u : usuarios) {
        out << u.username << ","
            << u.passwordHash << ","
            << (u.rol == RolUsuario::Administrador ? "Administrador" : "Usuario") << ","
            << (u.requierePassword ? "1" : "0") << "\n";
    }

    return true;
}

}