#include <catch2/catch_all.hpp>
#include <glm/glm.hpp>

using vec2 = glm::vec2;

TEST_CASE("Constructor", "[vec2]") {
    {
        // Default constructor
        vec2 v{};
        REQUIRE(v.x == 0.f);
        REQUIRE(v.y == 0.f);
    }

    {
        // Value constructor
        vec2 v(1.f, 2.f);
        REQUIRE(v.x == 1.f);
        REQUIRE(v.y == 2.f);
    }

    {
        // Braced initialize
        vec2 v{1.f, 2.f};
        REQUIRE(v.x == 1.f);
        REQUIRE(v.y == 2.f);
    }

    {
        // Copy constructor
        vec2 v(1.f, 2.f);
        vec2 v2(v);
        REQUIRE(v.x == v2.x);
        REQUIRE(v.y == v2.y);
    }
}

TEST_CASE("Assignment", "[vec2]") {
    vec2 v(1.f, 2.f);
    vec2 w = v;

    REQUIRE(v.x == w.x);
    REQUIRE(v.y == w.y);
}

TEST_CASE("Addition", "[vec2]") {
    vec2 v(1.f, 2.f);
    vec2 w(4.f, 8.f);

    SECTION("operator+  vector+vector") {
        vec2 u = v + w;
        REQUIRE(u.x == v.x + w.x);
        REQUIRE(u.y == v.y + w.y);

        vec2 u2 = w + v;
        REQUIRE(u2.x == w.x + v.x);
        REQUIRE(u2.y == w.y + v.y);
    }

    SECTION("operator+  vector+scalar") {
        vec2 u = v + 10.f;
        REQUIRE(u.x == v.x + 10.f);
        REQUIRE(u.y == v.y + 10.f);
    }

    SECTION("operator+=  vector+vector") {
        vec2 u = v;
        vec2 u2 = w;
        u += u2;
        REQUIRE(u.x == v.x + w.x);
        REQUIRE(u.y == v.y + w.y);

        REQUIRE(u2.x == w.x);
        REQUIRE(u2.y == w.y);
    }

    SECTION("operator+=  vector+scalar") {
        vec2 u = v;
        u += 10.f;
        REQUIRE(u.x == v.x + 10.f);
        REQUIRE(u.y == v.y + 10.f);
    }
}

TEST_CASE("Subtraction", "[vec2]") {
    vec2 v(1.f, 2.f);
    vec2 w(4.f, 8.f);

    SECTION("operator-  vector+vector") {
        vec2 u = v - w;
        REQUIRE(u.x == v.x - w.x);
        REQUIRE(u.y == v.y - w.y);

        vec2 u2 = w - v;
        REQUIRE(u2.x == w.x - v.x);
        REQUIRE(u2.y == w.y - v.y);
    }

    SECTION("operator-  vector+scalar") {
        vec2 u = v - 10.f;
        REQUIRE(u.x == v.x - 10.f);
        REQUIRE(u.y == v.y - 10.f);
    }

    SECTION("operator-=  vector+vector") {
        vec2 u = v;
        vec2 u2 = w;
        u -= u2;
        REQUIRE(u.x == v.x - w.x);
        REQUIRE(u.y == v.y - w.y);

        REQUIRE(u2.x == w.x);
        REQUIRE(u2.y == w.y);
    }

    SECTION("operator-=  vector+scalar") {
        vec2 u = v;
        u -= 10.f;
        REQUIRE(u.x == v.x - 10.f);
        REQUIRE(u.y == v.y - 10.f);
    }
}

TEST_CASE("Multiplication", "[vec2]") {
    vec2 v(1.f, 2.f);
    vec2 w(4.f, 8.f);

    SECTION("operator*  vector+vector") {
        vec2 u = v * w;
        REQUIRE(u.x == v.x * w.x);
        REQUIRE(u.y == v.y * w.y);

        vec2 u2 = w * v;
        REQUIRE(u2.x == w.x * v.x);
        REQUIRE(u2.y == w.y * v.y);
    }

    SECTION("operator*  vector+scalar") {
        vec2 u = v * 10.f;
        REQUIRE(u.x == v.x * 10.f);
        REQUIRE(u.y == v.y * 10.f);
    }

    SECTION("operator*=  vector+vector") {
        vec2 u = v;
        vec2 u2 = w;
        u *= u2;
        REQUIRE(u.x == v.x * w.x);
        REQUIRE(u.y == v.y * w.y);

        REQUIRE(u2.x == w.x);
        REQUIRE(u2.y == w.y);
    }

    SECTION("operator*=  vector+scalar") {
        vec2 u = v;
        u *= 10.f;
        REQUIRE(u.x == v.x * 10.f);
        REQUIRE(u.y == v.y * 10.f);
    }
}

TEST_CASE("Division", "[vec2]") {
    vec2 v(1.f, 2.f);
    vec2 w(4.f, 8.f);

    SECTION("operator/  vector+vector") {
        vec2 u = v / w;
        REQUIRE(u.x == v.x / w.x);
        REQUIRE(u.y == v.y / w.y);

        vec2 u2 = w / v;
        REQUIRE(u2.x == w.x / v.x);
        REQUIRE(u2.y == w.y / v.y);
    }

    SECTION("operator/  vector+scalar") {
        vec2 u = v / 10.f;
        REQUIRE(u.x == v.x / 10.f);
        REQUIRE(u.y == v.y / 10.f);
    }

    SECTION("operator/=  vector+vector") {
        vec2 u = v;
        vec2 u2 = w;
        u /= u2;
        REQUIRE(u.x == v.x / w.x);
        REQUIRE(u.y == v.y / w.y);

        REQUIRE(u2.x == w.x);
        REQUIRE(u2.y == w.y);
    }

    SECTION("operator/=  vector+scalar") {
        vec2 u = v;
        u /= 10.f;
        REQUIRE(u.x == v.x / 10.f);
        REQUIRE(u.y == v.y / 10.f);
    }
}

TEST_CASE("Length", "[vec2]") {
    vec2 v(1.f, 2.f);
    REQUIRE(glm::length(v) == Catch::Approx(std::sqrt(5.f)));

    vec2 w(5.f, 25.f);
    REQUIRE(glm::length(w) == Catch::Approx(std::sqrt(650.f)));
}

TEST_CASE("Normalize", "[vec2]") {
    vec2 v(1.f, 2.f);
    vec2 vn = glm::normalize(v);

    REQUIRE(vn.x == Catch::Approx(v.x / glm::length(v)));
    REQUIRE(vn.y == Catch::Approx(v.y / glm::length(v)));
    REQUIRE(glm::length(vn) == Catch::Approx(1.f));
}
