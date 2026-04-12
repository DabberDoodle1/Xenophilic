#ifndef ENTITY_HEADER
#define ENTITY_HEADER

class Entity {
public:
    Entity();
    ~Entity();

    void initialize();
    void bind() const;

    double pos_x;
    double pos_y;
    double vel_x;
    double vel_y;
private:
    unsigned int VAO;
};

#endif
