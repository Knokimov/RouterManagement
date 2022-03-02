void print(int router_id); // prints information about the router

void setFlag(int router_id, unsigned char flag, unsigned char value); // changes the flag of the router

void setModel(int router_id, char *model); // set the model of the router

void addConnection(int router_id1, int router_id2); // adds another router to the connections of the router

void deleteRouter(int router_id); // deletes the router from all routers connections

void checkRoute(int router_id1, int router_id2); // checks if a route exists from router1 to router2

void executeCommand(char *argv[]); // reads commands from a file and executes them

void writeToFile(char *filename); // writes current router topology to a file in binary (router info and then connections)