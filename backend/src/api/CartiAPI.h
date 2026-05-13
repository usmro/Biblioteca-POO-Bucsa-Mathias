#include <crow/app.h>
#include <crow/middlewares/cors.h>
#include "../db/Database.h"

void registerCartiRoutes(crow::App<crow::CORSHandler>& app, Database& db);