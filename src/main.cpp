#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

static sf::Font load_font() {
    sf::Font f;
    const char* paths[] = {
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/tahoma.ttf",
        "C:/Windows/Fonts/DejaVuSansMono.ttf",
    };
    for (auto p : paths) if (f.loadFromFile(p)) return f;
    std::fputs("[error] no font found\n", stderr);
    std::exit(1);
}

struct Particle {
    float x, y, vx, vy, tx, ty;
};

int main() {
    sf::RenderWindow window({1200, 750}, "living typo");
    window.setFramerateLimit(120);
    sf::Font font = load_font();

    // Settings
    float k = 6.0f;
    float damp = 3.0f;
    float mouse_r = 120.0f;
    float char_size = 50.0f;
    float sample_step = 3.0f; // one particle every N pixels
    std::string text = "living typography";

    std::vector<Particle> parts;

    auto rebuild = [&] {
        // Render text to texture
        sf::Text tmp(text, font, (unsigned)char_size);
        auto bounds = tmp.getLocalBounds();
        int tw = (int)std::ceil(bounds.width) + 4;
        int th = (int)std::ceil(bounds.height) + 4;
        if (tw < 1) tw = 1; if (th < 1) th = 1;

        sf::RenderTexture rt;
        if (!rt.create((unsigned)tw, (unsigned)th)) return;
        rt.clear(sf::Color::Black);
        tmp.setPosition(-bounds.left + 2, -bounds.top + 2);
        rt.draw(tmp);
        rt.display();

        sf::Image img = rt.getTexture().copyToImage();
        int w = (int)img.getSize().x, h = (int)img.getSize().y;

        std::vector<Particle> next;
        for (int y = 0; y < h; y += (int)sample_step) {
            for (int x = 0; x < w; x += (int)sample_step) {
                auto px = img.getPixel((unsigned)x, (unsigned)y);
                if (px.r > 20 || px.g > 20 || px.b > 20) {
                    float fx = (float)x - (float)w * 0.5f;
                    float fy = (float)y - (float)h * 0.5f;
                    // Find existing particle near this position
                    auto it = std::find_if(parts.begin(), parts.end(), [&](Particle& p) {
                        float dx = p.tx - fx, dy = p.ty - fy;
                        return dx * dx + dy * dy < 4.0f;
                    });
                    if (it != parts.end()) {
                        it->tx = fx; it->ty = fy;
                        next.push_back(*it);
                    } else {
                        next.push_back({fx, fy, 0, 0, fx, fy});
                    }
                }
            }
        }
        parts = std::move(next);
    };

    rebuild();

    sf::Clock clock;
    bool paused = false;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();
            if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::Escape) window.close();
                if (ev.key.code == sf::Keyboard::Space) paused = !paused;
                if (ev.key.code == sf::Keyboard::R) {
                    for (auto& p : parts) {
                        p.x = (float)(std::rand() % 800 - 400);
                        p.y = (float)(std::rand() % 500 - 250);
                        p.vx = p.vy = 0;
                    }
                }
            }
            if (ev.type == sf::Event::TextEntered) {
                if (ev.text.unicode == 8 && !text.empty()) {
                    text.pop_back();
                    rebuild();
                } else if (ev.text.unicode >= 32 && ev.text.unicode < 127) {
                    text += (char)ev.text.unicode;
                    rebuild();
                }
            }
        }

        if (!paused) {
            sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            float mx = mp.x - 600.0f, my = mp.y - 375.0f;

            for (auto& p : parts) {
                float fx = (p.tx - p.x) * k - p.vx * damp;
                float fy = (p.ty - p.y) * k - p.vy * damp;

                float dx = p.x - mx, dy = p.y - my;
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist < mouse_r && dist > 0.5f) {
                    float push = 2000.0f / (dist * dist);
                    fx += dx / dist * push;
                    fy += dy / dist * push;
                }

                p.vx += fx * dt;
                p.vy += fy * dt;
                p.x += p.vx * dt;
                p.y += p.vy * dt;
            }
        }

        window.clear(sf::Color(12, 14, 20));

        // Mouse indicator
        sf::Vector2f mp_screen = (sf::Vector2f)sf::Mouse::getPosition(window);
        sf::CircleShape ring(mouse_r);
        ring.setPosition(mp_screen.x - mouse_r, mp_screen.y - mouse_r);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineThickness(1.0f);
        ring.setOutlineColor(sf::Color(200, 210, 240, 40));
        window.draw(ring);

        // Draw trails
        std::vector<sf::Vertex> lines;
        for (auto& p : parts) {
            lines.emplace_back(sf::Vector2f(p.x + 600, p.y + 375), sf::Color(200, 210, 240, 40));
            lines.emplace_back(sf::Vector2f(p.x + 600 - p.vx, p.y + 375 - p.vy), sf::Color(200, 210, 240, 0));
        }
        if (!lines.empty()) window.draw(lines.data(), lines.size(), sf::Lines);

        // Draw particles as small circles
        for (auto& p : parts) {
            float speed = std::sqrt(p.vx * p.vx + p.vy * p.vy);
            sf::Uint8 r = 200, g = 210, b = 240;
            if (speed > 30.0f) {
                float t = std::min(1.0f, (speed - 30.0f) / 200.0f);
                r = (sf::Uint8)(200 + (int)(55 * t));
                g = (sf::Uint8)(210 - (int)(100 * t));
                b = (sf::Uint8)(240 - (int)(200 * t));
            }
            sf::CircleShape dot(1.8f, 6);
            dot.setPosition(p.x + 600 - 1.8f, p.y + 375 - 1.8f);
            dot.setFillColor({r, g, b, 220});
            window.draw(dot);
        }

        // Show particle count
        sf::Text info(std::to_string(parts.size()) + " particles | type to change | R to scatter",
                      font, 14);
        info.setPosition({10, 10});
        info.setFillColor({140, 150, 170});
        window.draw(info);

        window.display();
    }
}
