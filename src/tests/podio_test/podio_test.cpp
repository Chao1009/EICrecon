
// Copyright 2022, Jefferson Science Associates, LLC.
// Subject to the terms in the LICENSE file found in the top-level directory.
#include <iostream>
#include "services/io/podio/RootWriter.h"
#include "services/io/podio/EventStore.h"
#include <podio/ROOTReader.h>
#include <edm4eic/CalorimeterHitCollection.h>
#include <edm4eic/ClusterCollection.h>

void write_read_test() {
    std::cout << "Hello world!" << std::endl;
    eic::EventStore es;
    eic::ROOTWriter writer("test_out.root", &es);

    // Set up writer
    auto& hits = es.create<edm4eic::CalorimeterHitCollection>("MyFunHits");
    auto& clusters = es.create<edm4eic::ClusterCollection>("MyFunClusters");
    writer.registerForWrite("MyFunHits");
    writer.registerForWrite("MyFunClusters");

    // Save several events

    edm4eic::MutableCalorimeterHit hit1;
    hit1.setCellID(22);
    hit1.setEnergy(22.2);
    
    edm4eic::MutableCalorimeterHit hit2;
    hit2.setCellID(27);
    hit2.setEnergy(27.7);

    edm4eic::MutableCalorimeterHit hit3;
    hit3.setCellID(49);
    hit3.setEnergy(49.9);

    edm4eic::MutableCluster cluster1;
    cluster1.setEnergy(22.2+49.9);
    cluster1.setNhits(2);
    cluster1.addToHits(hit1);
    cluster1.addToHits(hit3);

    edm4eic::MutableCluster cluster2;
    cluster2.setEnergy(27.7);
    cluster2.setNhits(1);
    cluster2.addToHits(hit2);

    hits.push_back(hit1);
    hits.push_back(hit2);
    hits.push_back(hit3);

    clusters.push_back(cluster1);
    clusters.push_back(cluster2);

    writer.writeEvent();

    es.clearCollections();
    // es.clear();

    edm4eic::MutableCalorimeterHit hit4;
    hit4.setCellID(42);
    hit4.setEnergy(42.2);
    
    edm4eic::MutableCalorimeterHit hit5;
    hit5.setCellID(7);
    hit5.setEnergy(7.6);
    
    edm4eic::MutableCluster cluster3;
    cluster3.setEnergy(7.6);
    cluster3.setNhits(1);
    cluster3.addToHits(hit5);

    // auto& hits2 = es.get<edm4eic::CalorimeterHitCollection>("MyFunHits");
    // auto& clusters2 = es.get<edm4eic::ClusterCollection>("MyFunClusters");

    hits.push_back(hit4);
    hits.push_back(hit5);
    clusters.push_back(cluster3);

    writer.writeEvent();
    writer.finish();

    eic::EventStore es_in;

    podio::ROOTReader reader;
    reader.openFile("test_out.root");
    es_in.setReader(&reader);

    auto nevents = reader.getEntries();
    for (int i=0; i<nevents; ++i) {
        reader.goToEvent(i);
        // reader.readEvent();
        auto& hits_in = es_in.get<edm4eic::CalorimeterHitCollection>("MyFunHits");
        auto& clusters_in = es_in.get<edm4eic::ClusterCollection>("MyFunClusters");
        std::cout << "Event " << i << std::endl;
        std::cout << "Hits: " << hits_in.size() << std::endl;
        std::cout << "Clusters: " << clusters_in.size() << std::endl;
        std::cout << "Associations: " << clusters_in.size() << std::endl;
        for (auto cluster : clusters_in) {
            for (auto i=cluster.hits_begin(), end=cluster.hits_end(); i != end; ++i) {
                std::cout << i->getCellID() << " ";
            }
            std::cout << std::endl;
        }
        reader.endOfEvent();
        es_in.clear();
    }
}


int main() {
    write_read_test();
}