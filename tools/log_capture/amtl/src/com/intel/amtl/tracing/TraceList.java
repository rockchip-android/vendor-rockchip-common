/* Android AMTL
 *
 * Copyright (C) Intel 2015
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Nicolae Natea <nicolaex.natea@intel.com>
 */

package com.intel.amtl.tracing;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class TraceList {
    List<Tracer> mTracers = new ArrayList<Tracer>();

    public class Tracer {
        String mName = "";
        GeneralTracing mTrace;

        Tracer(String name, GeneralTracing trace) {
            mName = name;
            mTrace = trace;
        }

        public boolean matches(String name) {
            return (this.mName != null && mName.equals(name));
        }

        public boolean matches(Tracer tracer) {
            return (this.mName != null && mName.equals(tracer.getName()));
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj) {
                return true;
            }

            if (obj == null || (this.getClass() != obj.getClass())) {
                return false;
            }

            Tracer other = (Tracer) obj;
            return (this.mName != null && this.mName.equals(other.mName)
                    && this.mTrace != null && this.mTrace.equals(other.mTrace));
        }

        public String getName() {
            return mName;
        }

        public GeneralTracing getLogger() {
            return mTrace;
        }
    }

    public boolean registerTracer(Tracer t) {
        for (Iterator<Tracer> it = mTracers.iterator(); it.hasNext();) {
            TraceList.Tracer tracer = it.next();
            if (tracer.matches(t)) {
                mTracers.remove(tracer);
                break;
            }
        }

        mTracers.add(t);
        return true;
    }

    public boolean registerTracer(String name, GeneralTracing logger) {
        Tracer t = new Tracer(name, logger);

        for (Iterator<Tracer> it = mTracers.iterator(); it.hasNext();) {
            TraceList.Tracer tracer = it.next();
            if (tracer.matches(t)) {
                mTracers.remove(tracer);
                break;
            }
        }

        mTracers.add(t);
        return true;
    }

    public boolean unRegisterTracer(Tracer t) {
        for (Iterator<Tracer> it = mTracers.iterator(); it.hasNext();) {
            TraceList.Tracer tracer = it.next();
            if (tracer.matches(t)) {
                it.remove();
                break;
            }
        }
        return true;
    }

    public GeneralTracing findLogger(Tracer t) {
        for (Iterator<Tracer> it = mTracers.iterator(); it.hasNext();) {
            TraceList.Tracer tracer = it.next();
            if (tracer.matches(t)) {
                return tracer.getLogger();
            }
        }

        return null;
    }

    public Tracer findTracer(Tracer t) {
        for (Iterator<Tracer> it = mTracers.iterator(); it.hasNext();) {
            TraceList.Tracer tracer = it.next();
            if (tracer.matches(t)) {
                return tracer;
            }
        }

        return null;
    }

    public List<Tracer> getTracers() {
        return mTracers;
    }

    private boolean listContains(Tracer t) {
        for (Iterator<Tracer> it = mTracers.iterator(); it.hasNext();) {
            TraceList.Tracer tracer = it.next();
            if (tracer.equals(t)) {
                return true;
            }
        }

        return false;
    }


    public TraceList mergeTracers(TraceList tracers) {
        for (Iterator<Tracer> it = tracers.getTracers().iterator(); it.hasNext();) {
            TraceList.Tracer tracer = it.next();
            registerTracer(tracer);
        }

        return this;
    }

    public TraceList getUpdatedTracers(TraceList tracers) {
        TraceList retval = new TraceList();

        for (Iterator<Tracer> it = tracers.getTracers().iterator(); it.hasNext();) {
            TraceList.Tracer tracer = it.next();
            if (!listContains(tracer)) {
                retval.registerTracer(tracer);
            }
        }

        return retval;
    }

    public TraceList getRemovedTracers(TraceList tracers) {
        TraceList retval = new TraceList();

        for (Iterator<Tracer> it = tracers.getTracers().iterator(); it.hasNext();) {
            TraceList.Tracer tracer = it.next();
            if (!listContains(tracer)) {
                retval.registerTracer(tracer);
            }
        }

        return retval;
    }
}
