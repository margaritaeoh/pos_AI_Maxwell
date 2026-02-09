#ifndef VENTA_H
#define VENTA_H

#include <string>
#include <vector>
#include <cstdint>

namespace pos {

struct LineaVenta {
    int64_t productoId;
    double cantidad;
    double precioUnitario;
    double subtotal;
};

struct Venta {
    int64_t id;
    std::string fechaHora;
    std::string usuario;
    std::vector<LineaVenta> lineas;
    double total;
};

} // namespace pos

#endif // VENTA_H