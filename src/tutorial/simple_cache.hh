#ifndef __TUTORIAL_SIMPLE_CACHE_HH__
#define __TUTORIAL_SIMPLE_CACHE_HH__

#include <unordered_map>

#include "mem/mem_object.hh"
#include "params/SimpleCache.hh"

class SimpleCache : public MemObject
{
  private:

    class CPUSidePort : public SlavePort
    {
      private:
        /// Since this is a vector port, need to know what number this one is
        int id;

        /// The object that owns this object (SimpleCache)
        SimpleCache *owner;

        /// True if the port needs to send a retry req.
        bool needRetry;

        /// If we tried to send a packet and it was blocked, store it here
        PacketPtr blockedPacket;

      public:
        CPUSidePort(const std::string& name, int id, SimpleCache *owner) :
            SlavePort(name, owner), id(id), owner(owner), needRetry(false),
            blockedPacket(nullptr)
        { }

        void sendPacket(PacketPtr pkt);

        AddrRangeList getAddrRanges() const override;

        void trySendRetry();

      protected:
        Tick recvAtomic(PacketPtr pkt) override
        { panic("recvAtomic unimpl."); }

        void recvFunctional(PacketPtr pkt) override;

        bool recvTimingReq(PacketPtr pkt) override;

        void recvRespRetry() override;
    };

    class MemSidePort : public MasterPort
    {
      private:
        /// The object that owns this object (SimpleCache)
        SimpleCache *owner;

        /// If we tried to send a packet and it was blocked, store it here
        PacketPtr blockedPacket;

      public:
        MemSidePort(const std::string& name, SimpleCache *owner) :
            MasterPort(name, owner), owner(owner), blockedPacket(nullptr)
        { }

        void sendPacket(PacketPtr pkt);

      protected:
        bool recvTimingResp(PacketPtr pkt) override;

        void recvReqRetry() override;

        void recvRangeChange() override;
    };

    bool handleRequest(PacketPtr pkt, int port_id);

    bool handleResponse(PacketPtr pkt);

    void sendResponse(PacketPtr pkt);

    void handleFunctional(PacketPtr pkt);

    void accessTiming(PacketPtr pkt);

    bool accessFunctional(PacketPtr pkt);

    void insert(PacketPtr pkt);

    AddrRangeList getAddrRanges() const;

    void sendRangeChange() const;

    /// Latency to check the cache. Number of cycles for both hit and miss
    const Cycles latency;

    /// The block size for the cache
    const unsigned blockSize;

    /// Number of blocks in the cache (size of cache / block size)
    const unsigned capacity;

    /// Instantiation of the CPU-side port
    std::vector<CPUSidePort> cpuPorts;

    /// Instantiation of the memory-side port
    MemSidePort memPort;

    /// True if this cache is currently blocked waiting for a response.
    bool blocked;

    /// Packet that we are currently handling. Used for upgrading to larger
    /// cache line sizes
    PacketPtr outstandingPacket;

    /// The port to send the response when we recieve it back
    int waitingPortId;

    /// For tracking the miss latency
    Tick missTime;

    /// An incredibly simple cache storage. Maps block addresses to data
    std::unordered_map<Addr, uint8_t*> cacheStore;

    /// Cache statistics
    Stats::Scalar hits;
    Stats::Scalar misses;
    Stats::Histogram missLatency;
    Stats::Formula hitRatio;

  public:

    SimpleCache(SimpleCacheParams *params);

    virtual BaseMasterPort& getMasterPort(const std::string& if_name,
                                          PortID idx = InvalidPortID) override;

    virtual BaseSlavePort& getSlavePort(const std::string& if_name,
                                        PortID idx = InvalidPortID) override;

    void regStats() override;
};

#endif // __TUTORIAL_SIMPLE_CACHE_HH__
