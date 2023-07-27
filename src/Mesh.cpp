#include "Mesh.h"
using namespace std;
using namespace glm;

vector<Face*> Mesh::getNeighborFaces(Face* face) {
    vector<Face*> res;
    HalfEdge* halfedge = face->he;
    do { 
        res.push_back(halfedge->flip->face);
        halfedge = halfedge -> next;
    } while ( halfedge != face->he);
    return res;
}

void Mesh::printCoord(HalfEdge* h) {
    cout << "[" << h->src->coord.x << ", " <<  h->src->coord.y << ", " <<  h->src->coord.z << "] -> [" << h->next->src->coord.x << ", "<< h->next->src->coord.y << ", "<< h->next->src->coord.z << "]" << endl;  
}

Point *Mesh::getPoint(vec3 v) {
    for (auto pt : pts)
    {
        // find the element in pts <vec3>
        if (pt->coord == v)
        {
            return pt;
        }
    }
    return nullptr; // if cannot find the point
}

GLuint Mesh::getIndex(vector<Point *> vec, vec3 coordinate) {
    bool found = false;
    GLuint foundIndex =0;
    for (int i=0; i< vec.size(); i++) {
        if (length(vec[i]->coord - coordinate) < .00001f) {
            found= true;
            foundIndex= i;
        }
    }
    // cout << "found at index: " << foundIndex << endl;
    return found ? foundIndex: -1;
}

Mesh::Mesh(vector<vec3> vertices, vector<vec3> normals, vector<GLuint> indices) {
    map<Point*, map<Point*, HalfEdge*>> halfmap;
    cout << "vertices size: " << vertices.size() << endl;
    cout << "normals  size: " << normals.size() << endl;
    cout << "indices  size: " << indices.size() << endl;

    // prep inputs: vertices, normals, indices
    int i = 0;
    int j = 1;
    while ( j < vertices.size() / 3) {
        while (!match(i, j, vertices)) {
            j = j+1;
        }
        if( i + 1 < j ) {
            // swapgroup(i+1, j, v);
            iter_swap(vertices.begin() + 3*(i+1), vertices.begin() + 3*j);
            iter_swap(normals.begin() + 3*(i+1), normals.begin() + 3*j);
            iter_swap(vertices.begin() + 3*(i+1)+1, vertices.begin() + 3*j+1);
            iter_swap(normals.begin() + 3*(i+1)+1, normals.begin() + 3*j+1);
            iter_swap(vertices.begin() + 3*(i+1)+2, vertices.begin() + 3*j+2);
            iter_swap(normals.begin() + 3*(i+1)+2, normals.begin() + 3*j+2);
        }
        i = i+1;
        j = i+1;
    }

        i = 0;
j = 1;
int m_i1, m_i2, m_j1, m_j2, o_i, o_j;
while ( j < vertices.size() / 3) {
    m_i1 = m_i2 = m_j1 = m_j2 = o_i = o_j = 0;
    bool first = true;
    for( int k = 3*i; k < 3*(i+1); k++ ) {
        for( int l=3*j; l < 3*(j+1); l++ ){
            if (vertices[k]==vertices[l]) {
                if( !first ) {
                    m_i2 = k % 3;
                    m_j2 = l % 3;
                } else {
                    first = false;
                    m_i1 = k % 3;
                    m_j1 = l % 3;
                }
            }
        }
    }
    if ( o_i == m_i1 ) {
        o_i++;
    }
    if ( o_i == m_i2) {
        o_i++;
    }
    if ( o_j == m_j1 ) {
        o_j++;
    }
    if ( o_j == m_j2 ) {
        o_j++;
    }
    
    if( o_i == o_j ) {
        if( m_i1 == m_j1) {
            iter_swap(vertices.begin() +  3*j + m_j1, vertices.begin() + 3*j + m_j2);
            iter_swap(normals.begin() +  3*j + m_j1, normals.begin() + 3*j + m_j2);
        }
    } else {
        if (m_i1 == m_j1) {
            iter_swap(vertices.begin() + 3*j + m_j2, vertices.begin() + 3*j + o_j);
            iter_swap(normals.begin() + 3*j + m_j2, normals.begin() + 3*j + o_j);
            iter_swap(vertices.begin() + 3*j + m_j1, vertices.begin() + 3*j + m_j2);
            iter_swap(normals.begin() + 3*j + m_j1, normals.begin() + 3*j + m_j2);
        } else if (m_i2 == m_j2) {
            iter_swap(vertices.begin() + 3*j + m_j1, vertices.begin() + 3*j + o_j);
            iter_swap(normals.begin() + 3*j + m_j1, normals.begin() + 3*j + o_j);
            iter_swap(vertices.begin() + 3*j + m_j1, vertices.begin() + 3*j + m_j2);
            iter_swap(normals.begin() + 3*j + m_j1, normals.begin() + 3*j + m_j2);
        } else if (m_i1 == m_j2) {
            iter_swap(vertices.begin() + 3*j + m_j1, vertices.begin() + 3*j + o_j);
            iter_swap(normals.begin() + 3*j + m_j1, normals.begin() + 3*j + o_j);
        } else {
            iter_swap(vertices.begin() + 3*j + m_j2, vertices.begin() + 3*j + o_j);
            iter_swap(normals.begin() + 3*j + m_j2, normals.begin() + 3*j + o_j);
        }
    }
    i = i+1;
    j = i+1;
}
    // initialze pts
    for (auto vertex : vertices)
    {
        // if vertex is in pts, skip, otherwise put in pts
        //     vec3
        Point *vpoint = new Point();
        vpoint->coord = vertex;
        if( getIndex(pts, vertex) == -1 ) pts.push_back(vpoint);
    }
    cout << "pts     size: " << pts.size() << endl;
    
    HalfEdge *halfedge0;
    HalfEdge *halfedge0f;
    HalfEdge *halfedge1;
    HalfEdge *halfedge1f;
    HalfEdge *halfedge2;
    HalfEdge *halfedge2f;
    Face     *face;
    bool p0_p1 = false;
    bool p1_p0 = false;
    bool p1_p2 = false;
    bool p2_p1 = false;
    bool p2_p0 = false;
    bool p0_p2 = false;

    // initialize faces and hes
    // given the verteces[indices[i .. i+2]] to of them share the previous indices[i-3 .. i-1] except for the first three
    for (int i = 0; i < indices.size(); i += 3)
    {
        // i= 0, 3, 6, 9, ...
        // 3 new halfedges no dup
        // 1 new faces no dup
        // points are duplicated if generated
        //                    halfedge0
        // vertices[indices[i]] -->  vertices[indices[i+1]];
        //            \                /
        // halfedge2   \      <       / halfedge1
        //          vertices[indices[i+2]]
        face = new Face();
        
        Point *p0 = getPoint(vertices[indices[i]]);
        Point *p1 = getPoint(vertices[indices[i + 1]]);
        Point *p2 = getPoint(vertices[indices[i + 2]]);
        p0_p1 = halfmap.find(p0) != halfmap.cend() && halfmap[p0].find(p1) != halfmap[p0].cend();
        p1_p0 = halfmap.find(p1) != halfmap.cend() && halfmap[p1].find(p0) != halfmap[p1].cend();
        p1_p2 = halfmap.find(p1) != halfmap.cend() && halfmap[p1].find(p2) != halfmap[p1].cend();
        p2_p1 = halfmap.find(p2) != halfmap.cend() && halfmap[p2].find(p1) != halfmap[p2].cend();
        p2_p0 = halfmap.find(p2) != halfmap.cend() && halfmap[p2].find(p0) != halfmap[p2].cend();
        p0_p2 = halfmap.find(p0) != halfmap.cend() && halfmap[p0].find(p2) != halfmap[p0].cend();

        // initialize halfedges if not defined before in halfmap
        if (!p0_p1) {
            halfedge0 = new HalfEdge();
            hes.push_back(halfedge0);
            halfmap[p0][p1] = halfedge0;
        } else {
            halfedge0 = halfmap[p0][p1];
        }
        if (!p1_p0) {
            halfedge0f = new HalfEdge();
            hes.push_back(halfedge0f);
            halfmap[p1][p0] = halfedge0f;
        } else {
            halfedge0f = halfmap[p1][p0];
        }
        halfedge0->flip = halfedge0f;
        halfedge0f->flip = halfedge0;

        
        if ( !p1_p2 ) {
            halfedge1 = new HalfEdge();
            hes.push_back(halfedge1);
            halfmap[p1][p2] = halfedge1;
        } else {
            halfedge1 = halfmap[p1][p2];
        }
        if ( !p2_p1 ) {
            halfedge1f = new HalfEdge();
            hes.push_back(halfedge1f);
            halfmap[p2][p1] = halfedge1f;
        } else {
            halfedge1f = halfmap[p2][p1];
        }
        halfedge1->flip = halfedge1f;
        halfedge1f->flip = halfedge1;


        if ( !p2_p0 ) {
            halfedge2 = new HalfEdge();
            hes.push_back(halfedge2);
            halfmap[p2][p0] = halfedge2;
        } else {
            halfedge2 = halfmap[p2][p0];
        }
        if ( !p0_p2 ) {
            halfedge2f = new HalfEdge();
            hes.push_back(halfedge2f);
            halfmap[p0][p2] = halfedge2f;
        } else {
            halfedge2f = halfmap[p0][p2];
        }
        halfedge2->flip = halfedge2f;
        halfedge2f->flip = halfedge2;

        face->he = halfedge0;
        halfedge0->face = face;
        halfedge1->face = face;
        halfedge2->face = face;

        halfedge0->src = p0; // find Point whose coord is vertices[indices[i]].xyz
        halfedge1->src = p1;
        halfedge2->src = p2;

        halfedge0f->src = p1;
        halfedge1f->src = p2;
        halfedge2f->src = p0;

        halfedge0->next = halfedge1;
        halfedge1->next = halfedge2;
        halfedge2->next = halfedge0;

        p0->he = halfedge0;
        p1->he = halfedge1;
        p2->he = halfedge2;

        faces.push_back(face);
    }
    
    cout << "hes.size() should be    12 / 60: " << hes.size() << endl;
    cout << "faces.size(): should be 4  / 20: " << faces.size() << endl;
    cout << "pts.size(): shouldbe    4  / 12 :" << pts.size() << endl;
};
    
void Mesh::printMap(map<Point*, map<Point*, HalfEdge*>> map) {
    cout<< "printing map.."<< endl;
    for (auto it1 = map.cbegin(); it1!= map.cend(); it1++ ) {
        for(auto it2 = it1->second.cbegin(); it2 != it1->second.cend(); it2++ ) {
            vec3 emptyC = vec3(99.0f,99.0f,99.0f);
            vec3 coord1 = it1->first?it1->first->coord:emptyC;
            vec3 coord2 = it2->first?it2->first->coord:emptyC;
            if ( it2->second ){
                if (it2->second->src) {
                vec3 coord3 = it2->second->src->coord;
                cout <<it2->second << "     [" << coord1.x << ", "<<coord1.y << ", " << coord1.z << "->"  << coord2.x <<", " << coord2.y <<", " << coord2.z << "]" 
                << "  src: (" << coord3.x << ", " << coord3.y << ", " << coord3.z<< ") next: " << it2->second->next << " flip: " << it2->second->flip << " face: " << it2->second->face << endl;
            }}
            else {cout <<it2->second << "     [" << coord1.x << ", "<<coord1.y << ", " << coord1.z << "->"  << coord2.x <<", " << coord2.y <<", " << coord2.z << "]"<< endl;}              
        }
    }
    cout<< "done" << endl;
}

Mesh *Mesh::Subdiv(int subdivLevel) {
    for (int i = 0; i < subdivLevel; i++)
        Subdivstep();
    out_vertices.clear();
    out_normals.clear();
    out_indices.clear();
    for (auto face : faces) {
        HalfEdge *h = face->he;
        do {
            vec3 v = h->src->coord;
            out_vertices.push_back(v);                    // check if out_vertices has getVec3(h->src)
            // out_indices.push_back(index++);//getIndex(out_vertices, v)); // find v in out_vertices and put its location in out_indices
            out_normals.push_back(face->getNormal());
            h = h->next;
        } while (h != face->he);
    }

    // indices
    if (subdivLevel) {
        for( GLuint i = 0; i < faces.size(); i++ ) {
            out_indices.push_back(4*i);//[6*i] = 4 * i;
            out_indices.push_back(4*i + 1);//6*i+1] = 4 * i + 1;
            out_indices.push_back(4*i+2);//[6*i+2] = 4 * i + 2;
            out_indices.push_back(4 * i);//[6*i+3] = 4 * i;
            out_indices.push_back(4*i+2);//[6*i+4] = 4 * i + 2;
            out_indices.push_back(4*i+3);//[6*i+5] = 4 * i + 3;
        }
    } else {
        for( GLuint i = 0; i < faces.size(); i++ ) {
            out_indices.push_back(3*i);
            out_indices.push_back(3*i + 1);
            out_indices.push_back(3*i+2);
        }
    }

    return this;
}

void Mesh::Subdivstep() {
    cout << "starting subdiv step.." << endl;
    // hescheck();
    vector<Point *>    newpts;    // size should be 62
    vector<HalfEdge *> newhes;    // size should be 240
    vector<Face *>     newfaces;  // size should be 60

    map<Face *, Point *>     fps;
    map<HalfEdge *, Point *> hps;
    map<Point*, map<Point*, HalfEdge*>> halfmap;
    Point    *         newV     = nullptr;
    Point    *         edgepoint= nullptr;
    Point    *         facepoint= nullptr;
    
    HalfEdge *         he0;
    HalfEdge *         he;
    vector<Point*>     facepoints;
    vector<Point*>     edgepoints;



    vec3 facesum = vec3(0.0f,0.0f,0.0f);
    vec3 faceaverage = vec3(0.0f,0.0f,0.0f);
    vec3 midpointsum = vec3(0.0f,0.0f,0.0f);
    vec3 midpointaverage = vec3(0.0f,0.0f,0.0f);
    vector<vec3> midpointCoordinates;
    int countedgepoint= 0;
    for (auto point : pts)
    {
        // get the endpoints of given Point
        newV = new Point();
        newpts.push_back(newV);
        // countp0++;
        // prevFP = nullptr;

        HalfEdge* he0 = point->he;
        HalfEdge* he = he0;


        float valence = 0.0f;

        facepoints.clear();
        edgepoints.clear();
        int newfaceBase = newfaces.size();
        midpointCoordinates.clear();
        do {
            // midpointCoordinate
            midpointCoordinates.push_back((he->src->coord+ he->flip->src->coord)/2.0f);

            // new faces
            Face *newface = new Face();
            // newface->he = he;  // why the r value is he? it is from the old mesh
            newfaces.push_back(newface);

            // putting facepoints in order oldface
            if( fps.find(he->face) != fps.end() ){
                facepoint = fps[he->face];
            } else {
                facepoint = new Point();           // ㅇ
                newpts.push_back(facepoint);
                facepoint->coord = he->face->facePointCoord();
                fps[he->face] = facepoint;
            }
            facepoints.push_back(facepoint);
            
            // putting edgepoints in order
            if (hps.find(he) != hps.end() ) {
                edgepoint = hps[he];
            } else {
                edgepoint = new Point();        // ㅁ
                newpts.push_back(edgepoint);
                edgepoint->coord = facepoint->coord + he->flip->face->facePointCoord() + he->src->coord + he->flip->src->coord;
                edgepoint->coord = edgepoint->coord / 4.0f;
                hps[he]          = edgepoint;
                hps[he->flip]    = edgepoint;
            }
            edgepoints.push_back(edgepoint);
            
            valence++;
            he = he->flip->next;
        } while( he != he0 );

        // facepoints:: vector<Point*>
        // edgepoints:: vector<Point*>

        int size = round(valence);
        // flip  initialize if needed
        for( int i=0; i< size; i++) {
            if(halfmap.find(facepoints[i]) != halfmap.cend() && halfmap[facepoints[i]].find(edgepoints[i]) != halfmap[facepoints[i]].cend()){
                cout << "already f_i -> e_i exists where i is: " << i << endl; // printCoord(halfmap[facepoints[i]][edgepoints[i]]);
            } else {
                    halfmap                 [facepoints[i]][edgepoints[i]] = new HalfEdge();
                    newhes.push_back(halfmap[facepoints[i]][edgepoints[i]]);
            }
            if(halfmap.find(edgepoints[i]) != halfmap.cend() && halfmap[edgepoints[i]].find(facepoints[i]) != halfmap[edgepoints[i]].cend()) {
                cout << "already e_i -> f_i exists where i is: " << i << endl; //  printCoord(halfmap[edgepoints[i]][facepoints[i]]);
            } else {
                halfmap                 [edgepoints[i]][facepoints[i]] = new HalfEdge();
                newhes.push_back(halfmap[edgepoints[i]][facepoints[i]]);
            }
            halfmap                     [facepoints[i]][edgepoints[i]]->flip = halfmap[edgepoints[i]][facepoints[i]];
            halfmap                     [edgepoints[i]][facepoints[i]]->flip = halfmap[facepoints[i]][edgepoints[i]];

            if(halfmap.find(edgepoints[i]) != halfmap.cend() && halfmap[edgepoints[i]].find(facepoints[(i+1)%size]) != halfmap[edgepoints[i]].cend() ) {
                cout << "already e_i -> f_(i+1)modsize exist where i is " << i << endl; // printCoord(halfmap[facepoints[i]][edgepoints[(i+1)%size]]);
            } else {
                halfmap                 [edgepoints[i]][facepoints[(i+1)%size]] = new HalfEdge();
                newhes.push_back(halfmap[edgepoints[i]][facepoints[(i+1)%size]]);
            }
            if(halfmap.find(facepoints[(i+1)%size]) != halfmap.cend() && halfmap[facepoints[(i+1)%size]].find(edgepoints[i]) != halfmap[facepoints[(i+1)%size]].cend()) {
                cout << "already f_(i+1)modsize->e_i exists where i is " << i << endl; 
            } else { 
                halfmap                 [facepoints[(i+1)%size]][edgepoints[i]] = new HalfEdge();
                newhes.push_back(halfmap[facepoints[(i+1)%size]][edgepoints[i]]);
            }
            halfmap                     [edgepoints[i]][facepoints[(i+1)%size]]->flip = halfmap[facepoints[(i+1)%size]][edgepoints[i]];
            halfmap                     [facepoints[(i+1)%size]][edgepoints[i]]->flip = halfmap[edgepoints[i]][facepoints[(i+1)%size]];

            if(halfmap.find(newV) != halfmap.cend() && halfmap[newV].find(edgepoints[i]) != halfmap[newV].cend()) {
            } else {
                halfmap                 [newV][edgepoints[i]] = new HalfEdge();
                newhes.push_back(halfmap[newV][edgepoints[i]]);
            }
            if(halfmap.find(edgepoints[i]) != halfmap.cend() && halfmap[edgepoints[i]].find(newV) != halfmap[edgepoints[i]].cend()){
            } else {
                halfmap                 [edgepoints[i]][newV] = new HalfEdge();
                newhes.push_back(halfmap[edgepoints[i]][newV]);
            }
            halfmap                     [newV][edgepoints[i]]->flip = halfmap[edgepoints[i]][newV];
            halfmap                     [edgepoints[i]][newV]->flip = halfmap[newV][edgepoints[i]];
        }


        // src
        for( int i=0; i< size; i++) {
            halfmap[edgepoints[i]][facepoints[i]]->src = edgepoints[i];
            halfmap[facepoints[i]][edgepoints[i]]->src = facepoints[i];
            halfmap[edgepoints[i]][facepoints[(i+1)%size]]->src=edgepoints[i];
            halfmap[facepoints[(i+1)%size]][edgepoints[i]]->src=facepoints[(i+1)%size];
            halfmap[newV][edgepoints[i]]->src = newV;
            halfmap[edgepoints[i]][newV]->src = edgepoints[i];
        }

        // next
        for ( int i=0; i< size; i++) {
            halfmap[edgepoints[i]][facepoints[i]]         ->next = halfmap[facepoints[i]][edgepoints[(i+size-1)%size]];
            halfmap[facepoints[(i+1)%size]][edgepoints[i]]->next = halfmap[edgepoints[i]]         [newV];
            halfmap[newV][edgepoints[i]]                  ->next = halfmap[edgepoints[i]]         [facepoints[i]];
            halfmap[edgepoints[i]][newV]                  ->next = halfmap[newV]         [edgepoints[(i+1)%size]];
        }

        // face
        for ( int i=0; i< size; i++) {
            halfmap[newV][edgepoints[i]]                  -> face = newfaces[newfaceBase+i];
            halfmap[edgepoints[i]][facepoints[i]]         -> face = newfaces[newfaceBase+i];
            halfmap[facepoints[(i+1)%size]][edgepoints[i]]-> face = newfaces[newfaceBase+(i + 1) % size];
            halfmap[edgepoints[i]][newV]         -> face = newfaces[newfaceBase+(i + 1) % size];
        }

        // pts->he
        newV->he = halfmap[newV][edgepoints[0]];
        for ( int i=0; i< size; i++) {
            edgepoints[i]-> he = halfmap[edgepoints[i]][newV];
            facepoints[i]-> he = halfmap[facepoints[i]][edgepoints[i]];
        }

        // newfaces  -> he
        for ( int i=0; i< size; i++) {
            newfaces[newfaceBase + i]->he = halfmap[newV][edgepoints[i]];
        }

        facesum = vec3(0.0f,0.0f,0.0f);
        faceaverage = vec3(0.0f,0.0f,0.0f);
        midpointsum = vec3(0.0f,0.0f,0.0f);
        midpointaverage = vec3(0.0f,0.0f,0.0f);
        
        // calculate newV-<coord
        for ( int i=0; i< size; i++) {
            facesum     += facepoints[i]->coord;
            midpointsum += midpointCoordinates[i];
        }
        faceaverage     = facesum     / valence;
        midpointaverage = midpointsum / valence;
        newV->coord = (faceaverage + 2.0f * midpointaverage + he0->src->coord* (valence - 3.0f))/valence;


        cout << "fps size(20): " << fps.size() << endl;
        cout << "hps size(60): " << hps.size() << endl;
        cout << "newhessize(62):" << newhes.size() << endl; 
        
        cout << "valence: " << valence << endl;
        faceaverage = facesum / valence; // looks good
        midpointaverage = midpointsum / valence; // looks good
        // get updated vertex
        newV->coord = faceaverage + midpointaverage * 2.0f  + point->coord * (valence - 3);
        newV->coord /= valence;
    } // end of for      12 iterations

    printMap(halfmap);
    cout << "countedgepoint (#oldedge):" << countedgepoint<< endl;
    cout << "newhes.size() should be   240:" << newhes.size()  << endl;    // why 251 not 240 which is expected nullptr was the culprit where I should not nullptr
    cout << "newfaces.size() should be 60:" << newfaces.size() << endl;    // done 
    cout << "newpts.size() should be   62:" << newpts.size()   << endl;    // curse of float
    cout << "halfmap.size() should be  50:" << halfmap.size() << endl;
    pts = newpts;
    hes = newhes;
    faces = newfaces;
}
